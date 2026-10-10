# -*- coding: utf-8 -*-
"""
e_mcp_core.py — 易语言静态编译程序解析引擎 (IDA 9.4 IDAPython 版)

移植自 fjqisba/E-Decompiler (C++ IDA 7.5 插件), 原项目地址:
https://github.com/fjqisba/E-Decompiler

本模块不依赖 IDA SDK, 只依赖 IDAPython (ida_bytes / ida_funcs / ida_name /
ida_segment / ida_ua / ida_allins), 因此可以在 IDA 9.x 上直接运行。

功能:
  * 识别易语言静态编译特征, 定位用户代码段
  * 解析支持库 / 数据类型 / 库命令表
  * 用 plugins\\esig 特征码识别核心支持库函数并命名
  * 定位 krnln 跳转表 (DLL命令/错误回调/读写组件属性等) 并命名
  * 解析窗口/控件/菜单资源与事件处理函数, 在 IDB 中命名 (如 _按钮1_被单击)
  * 解析用户导入表 (DLL 命令)
  * 扫描易语言类虚表

所有解析结果保存在 module 级的 `STATE` 中, 供 MCP 工具查询。
"""

import json
import os
import re
import struct

import ida_bytes
import ida_funcs
import ida_name
import ida_segment
import ida_ua
import ida_allins
import ida_auto
import ida_idaapi
import ida_kernwin

# ---------------------------------------------------------------------------
# 事件名称表: 由原项目 EAppControl/*.cpp 自动提取 (event_tables.json)
# ---------------------------------------------------------------------------

_HERE = os.path.dirname(os.path.abspath(__file__))
_EVENT_JSON = os.path.join(_HERE, "event_tables.json")

def _load_event_tables():
    try:
        with open(_EVENT_JSON, "r", encoding="utf-8") as f:
            return json.load(f)
    except Exception:
        return {"common": {}, "controls": {}}

EVENT_TABLES = _load_event_tables()

# 控件类型名 -> 事件表类名 (原项目 CKrnl_* / CIext2_* 类)
_CONTROL_EVENT_MAP = {
    "窗口": "CKrnl_window",
    "按钮": "CKrnl_Button",
    "编辑框": "CKrnl_EditBox",
    "标签": "CKrnl_Label",
    "选择框": "CKrnl_CheckBox",
    "单选框": "CKrnl_RadioBox",
    "组合框": "CKrnl_ComboBox",
    "列表框": "CKrnl_ListBox",
    "选择列表框": "CKrnl_ChkListBox",
    "图片框": "CKrnl_PicBox",
    "外形框": "CKrnl_ShapeBox",
    "画板": "CKrnl_DrawPanel",
    "分组框": "CKrnl_GroupBox",
    "横向滚动条": "CKrnl_HScrollBar",
    "纵向滚动条": "CKrnl_VScrollBar",
    "进度条": "CKrnl_ProcessBar",
    "滑块条": "CKrnl_SliderBar",
    "选择夹": "CKrnl_Tab",
    "影像框": "CKrnl_AnimateBox",
    "时钟": "CKrnl_Timer",
    "图形按钮": "CKrnl_PicBtn",
    "拖放对象": "CKrnl_DropTarget",
    "卡通动画框": "CIext2_CartoonBox",
    "IP编辑框": "CIext2_IPEditBox",
    "超级编辑框": "CIext2_RichEdit",
    "分割条": "CIext2_SplitterBar",
    "高级影像框": "CIext2_SuperAnimateBox",
    "高级按钮": "CIext2_SuperBtn",
}

KRNL_JMP_NAMES = {
    "Jmp_MReportError": "错误回调",
    "Jmp_MCallDllCmd": "DLL命令",
    "Jmp_MCallLibCmd": "三方支持库命令",
    "Jmp_MCallKrnlLibCmd": "核心支持库命令",
    "Jmp_MReadProperty": "读取组件属性",
    "Jmp_MWriteProperty": "设置组件属性",
    "Jmp_MMalloc": "分配内存",
    "Jmp_MRealloc": "重新分配内存",
    "Jmp_MFree": "释放内存",
    "Jmp_MExitProcess": "结束",
    "Jmp_MMessageLoop": "窗口消息循环",
    "Jmp_MLoadBeginWin": "载入启动窗口",
    "Jmp_MOtherHelp": "辅助函数",
}

KRNL_CALL_FIELDS = [
    "krnl_MReportError", "krnl_MCallDllCmd", "krnl_MCallLibCmd",
    "krnl_MCallKrnlLibCmd", "krnl_MReadProperty", "krnl_MWriteProperty",
    "krnl_MMalloc", "krnl_MRealloc", "krnl_MFree", "krnl_MExitProcess",
    "krnl_MMessageLoop", "krnl_MLoadBeginWin", "krnl_MOtherHelp",
]

# 控件索引: (windowId, controlId) -> 控件dict, analyze() 后填充
CONTROL_INDEX = {}

# 全局解析结果
STATE = {
    "analyzed": False,
    "error": None,
    "info": {},           # 概览: 代码段/函数数/窗口数等
    "libs": [],           # 支持库列表
    "controls": [],       # 控件列表
    "imports": [],        # 用户导入表
    "functions": {},      # ea -> {"name":..., "type":...}
    "krnl": {},           # krnlJmp
    "vtables": [],        # 类虚表
    "unmatched_cmds": [], # 特征码未命中的库命令
}


# ---------------------------------------------------------------------------
# 内存模型 (对应原项目 SectionManager): 把所有段读进一个大缓冲区,
# 之后全部解析都在缓冲区上进行, 速度快且不依赖 SDK 搜索 API。
# ---------------------------------------------------------------------------

class Memory:
    def __init__(self):
        self.base = 0
        self.buf = b""
        self._load()

    def _load(self):
        chunks = []
        segs = []
        n = ida_segment.get_segm_qty()
        for i in range(n):
            seg = ida_segment.getnseg(i)
            if seg is None:
                continue
            size = seg.size()
            data = ida_bytes.get_bytes(seg.start_ea, size, True)
            if data is None:
                data = b"\x00" * size
            segs.append((seg.start_ea, size))
            chunks.append(data)
        self.buf = b"".join(chunks)
        self.base = segs[0][0] if segs else 0

    def off(self, linear):
        """线性地址 -> 缓冲区偏移, 失败返回 -1"""
        off = linear - self.base
        if off < 0 or off >= len(self.buf):
            return -1
        return off

    def lin(self, off):
        return self.base + off

    def u8(self, off):
        return self.buf[off]

    def u32(self, off):
        return struct.unpack_from("<I", self.buf, off)[0]

    def i32(self, off):
        return struct.unpack_from("<i", self.buf, off)[0]

    def u16(self, off):
        return struct.unpack_from("<H", self.buf, off)[0]

    def cstr(self, off, max_len=1024):
        """以 \\0 结尾的 GBK 字符串"""
        if off < 0 or off >= len(self.buf):
            return ""
        end = self.buf.find(b"\x00", off, off + max_len)
        if end == -1:
            end = min(off + max_len, len(self.buf))
        raw = self.buf[off:end]
        try:
            return raw.decode("gbk", errors="replace")
        except Exception:
            return raw.decode("latin1", errors="replace")

    def search(self, pattern, start_off=0, end_off=None):
        """带通配符搜索: pattern 为 bytes, b"?" 表示单字节通配。
        返回缓冲区偏移, 找不到返回 -1"""
        if end_off is None:
            end_off = len(self.buf)
        regex = b"".join(b"." if p == ord("?") else re.escape(bytes([p])) for p in pattern)
        m = re.compile(regex, re.S).search(self.buf, start_off, end_off)
        return m.start() if m else -1


# ---------------------------------------------------------------------------
# esig 特征码引擎 (对应原项目 TrieTree)
# ---------------------------------------------------------------------------

NODE_NORMAL = 0
NODE_LONGJMP = 1
NODE_CALL = 2
NODE_CALLAPI = 3
NODE_JMPAPI = 4
NODE_CONSTANT = 5
NODE_LEFTPASS = 6
NODE_RIGHTPASS = 7
NODE_ALLPASS = 8

HEXVAL = {ord(c): i for i, c in enumerate("0123456789abcdefABCDEF")}
_HEXMAP = {c: i for i, c in enumerate("0123456789abcdef")}
_HEXMAP.update({c.upper(): i for c, i in list(_HEXMAP.items())})


def _hex_to_bin(hi, lo):
    return (_HEXMAP[hi] << 4) | _HEXMAP[lo]


class Node:
    __slots__ = ("children", "special", "text", "func_name", "stype")

    def __init__(self, stype=NODE_NORMAL):
        self.children = [None] * 256
        self.special = []      # list[Node]
        self.text = ""
        self.func_name = None
        self.stype = stype


class TrieTree:
    """易语言特征码匹配树, esig 文本格式:
    *****SubFunc***** 段: 子函数名:特征串
    ***Func*** 段: 函数名:特征串
    特征串语法: 十六进制字节 / ?? 全通配 / ?x 半字节通配 / x? 半字节通配 /
    <子函数名> CALL 子函数 / <[dll.api]> CALL API / [dll.api] JMP API /
    -> 长跳转 / !子函数名! 常量指针指向子函数
    """

    def __init__(self):
        self.root = Node()
        self.sub_funcs = {}
        self.rfunc = {}

    @staticmethod
    def _get_mid(src, left, right):
        a = src.find(left)
        if a == -1:
            return ""
        a += len(left)
        b = src.find(right, a)
        if b == -1:
            return ""
        return src[a:b]

    def load_sig(self, path):
        try:
            with open(path, "rb") as f:
                text = f.read().decode("gbk", errors="replace")
        except OSError:
            return False
        text = text.replace("\r\n", "\n")

        sub = self._get_mid(text, "*****SubFunc*****\n", "*****SubFunc_End*****")
        for line in sub.split("\n"):
            if not line:
                continue
            name, _, sig = line.partition(":")
            if sig:
                self.sub_funcs[name] = sig

        func = self._get_mid(text, "***Func***\n", "***Func_End***")
        for line in func.split("\n"):
            if not line:
                continue
            name, _, sig = line.partition(":")
            if sig:
                self._insert(sig, name)
        return True

    def _add_node(self, p, txt):
        idx = _hex_to_bin(txt[0], txt[1])
        if p.children[idx] is None:
            node = Node()
            node.text = txt
            p.children[idx] = node
        return p.children[idx]

    def _add_special(self, p, stype, txt):
        for node in p.special:
            if node.stype == stype and node.text == txt:
                return node
        node = Node(stype)
        node.text = txt
        p.special.append(node)
        return node

    def _insert(self, sig, name):
        p = self.root
        i, n = 0, len(sig)
        while i < n:
            c = sig[i]
            if c == "-":
                if sig[i + 1:i + 3] == "->":
                    p = self._add_node(p, "E9")
                    p = self._add_special(p, NODE_LONGJMP, "")
                    i += 3
                    continue
                return False
            if c == "<":
                if sig[i + 1] == "[":
                    post = sig.find("]>", i)
                    if post == -1:
                        return False
                    p = self._add_node(p, "FF")
                    p = self._add_node(p, "15")
                    p = self._add_special(p, NODE_CALLAPI, sig[i + 2:post])
                    i = post + 2
                    continue
                post = sig.find(">", i)
                if post == -1:
                    return False
                p = self._add_node(p, "E8")
                p = self._add_special(p, NODE_CALL, sig[i + 1:post])
                i = post + 1
                continue
            if c == "[":
                post = sig.find("]", i)
                if post == -1:
                    return False
                p = self._add_node(p, "FF")
                p = self._add_node(p, "25")
                p = self._add_special(p, NODE_JMPAPI, sig[i + 1:post])
                i = post + 1
                continue
            if c == "?":
                if i + 1 < n and sig[i + 1] == "?":
                    p = self._add_special(p, NODE_ALLPASS, "??")
                else:
                    p = self._add_special(p, NODE_LEFTPASS, sig[i:i + 2])
                i += 2
                continue
            if c == "!":
                post = sig.find("!", i + 1)
                if post == -1:
                    return False
                p = self._add_special(p, NODE_CONSTANT, sig[i + 1:post])
                i = post + 1
                continue
            # 普通字节 / 右半字节通配
            if i + 1 < n and sig[i + 1] == "?":
                p = self._add_special(p, NODE_RIGHTPASS, sig[i:i + 2])
                i += 2
            else:
                p = self._add_node(p, sig[i:i + 2])
                i += 2
        if p.func_name:
            return False
        p.func_name = name
        return True

    # -- 匹配 ----------------------------------------------------------------

    def _slow_match(self, mem, off, sig):
        """在内存缓冲区上按特征串逐字节匹配 (对应原项目 SlowMatch)"""
        i, n = 0, len(sig)
        while i < n:
            c = sig[i]
            if c == "-":
                if sig[i + 1:i + 3] == "->":
                    if mem.u8(off) != 0xE9:
                        return -1
                    off = mem.off(mem.lin(off) + mem.i32(off + 1) + 5)
                    if off < 0:
                        return -1
                    i += 3
                    continue
                return -1
            if c == "<":
                if sig[i + 1] == "[":
                    post = sig.find("]>", i)
                    if post == -1:
                        return -1
                    if not self._cmp_api(mem, off, sig[i + 2:post]):
                        return -1
                    off += 6
                    i = post + 2
                    continue
                post = sig.find(">", i)
                if post == -1:
                    return -1
                if not self._cmp_call(mem, off, sig[i + 1:post]):
                    return -1
                off += 5
                i = post + 1
                continue
            if c == "[":
                post = sig.find("]", i)
                if post == -1:
                    return -1
                if not self._cmp_api(mem, off, sig[i + 1:post]):
                    return -1
                off += 6
                i = post + 1
                continue
            if c == "!":
                post = sig.find("!", i + 1)
                if post == -1:
                    return -1
                const_name = sig[i + 1:post]
                oaddr = mem.u32(off)
                if self.rfunc.get(oaddr) == const_name or \
                        self._slow_match(mem, mem.off(oaddr), self.sub_funcs.get(const_name, "")) != -1:
                    off += 4
                    i = post + 1
                    continue
                return -1
            if c == "?":
                if i + 1 < n and sig[i + 1] == "?":
                    off += 1
                    i += 2
                    continue
                if (mem.u8(off) & 0xF) == _HEXMAP.get(sig[i + 1], -1):
                    off += 1
                    i += 2
                    continue
                return -1
            if i + 1 < n and sig[i + 1] == "?":
                if (mem.u8(off) >> 4) == _HEXMAP.get(c, -1):
                    off += 1
                    i += 2
                    continue
                return -1
            if mem.u8(off) != _hex_to_bin(c, sig[i + 1]):
                return -1
            off += 1
            i += 2
        return off

    def _cmp_call(self, mem, off, func_name):
        if mem.u8(off) != 0xE8:
            return False
        oaddr = mem.lin(off) + mem.i32(off + 1) + 5
        if self.rfunc.get(oaddr) == func_name:
            return True
        sub = self.sub_funcs.get(func_name, "")
        target = mem.off(oaddr)
        if sub and target >= 0 and self._slow_match(mem, target, sub) != -1:
            self.rfunc[oaddr] = func_name
            return True
        return False

    def _cmp_api(self, mem, off, iat_eat):
        """FF 15 / FF 25 后跟 IAT 指针, 校验指针处名字是否匹配"""
        if mem.u8(off) != 0xFF:
            return False
        if mem.u8(off + 1) not in (0x15, 0x25):
            return False
        if "||" in iat_eat:
            iat_com, eat_com = iat_eat.split("||", 1)
        else:
            iat_com = iat_eat
            eat_com = iat_eat.split(".", 1)[-1]
        if "." in iat_com:
            iat_com = iat_com.split(".", 1)[-1]
        oaddr = mem.u32(off + 2)
        name = ida_name.get_name(oaddr) or ""
        if name.startswith("__imp_"):
            name = name[6:]
        return name in (eat_com, iat_com)

    def _fast_match(self, mem, node, off):
        """处理特殊节点, 返回新偏移或 -1 (对应原项目 FastMatch)"""
        st = node.stype
        if st == NODE_NORMAL:
            return off
        if st == NODE_LONGJMP:
            target = mem.lin(off - 1) + mem.i32(off) + 5
            noff = mem.off(target)
            return noff
        if st == NODE_CALL:
            # off 指向 E8 之后的 4 字节位移
            oaddr = mem.lin(off) + mem.i32(off) + 5
            if self.rfunc.get(oaddr) == node.text:
                return off + 4
            sub = self.sub_funcs.get(node.text, "")
            target = mem.off(oaddr)
            if sub and target >= 0 and self._slow_match(mem, target, sub) != -1:
                self.rfunc[oaddr] = node.text
                return off + 4
            return -1
        if st in (NODE_JMPAPI, NODE_CALLAPI):
            if self._cmp_api(mem, off, node.text):
                return off + 4
            return -1
        if st == NODE_CONSTANT:
            oaddr = mem.u32(off)
            if self.rfunc.get(oaddr) == node.text:
                return off + 4
            target = mem.off(oaddr)
            sub = self.sub_funcs.get(node.text, "")
            if sub and target >= 0 and self._slow_match(mem, target, sub) != -1:
                self.rfunc[oaddr] = node.text
                return off + 4
            return -1
        if st == NODE_LEFTPASS:
            if (mem.u8(off) & 0xF) == _HEXMAP.get(node.text[1], -1):
                return off + 1
            return -1
        if st == NODE_RIGHTPASS:
            if (mem.u8(off) >> 4) == _HEXMAP.get(node.text[0], -1):
                return off + 1
            return -1
        if st == NODE_ALLPASS:
            return off + 1
        return -1

    def match_func(self, mem, off):
        """从函数入口开始匹配, 命中返回函数名, 否则 None"""
        stack = [(self.root, off)]
        while stack:
            node, cur = stack.pop()
            new_off = self._fast_match(mem, node, cur)
            if new_off == -1:
                continue
            if node.func_name:
                return node.func_name
            for sp in node.special:
                stack.append((sp, new_off))
            child = node.children[mem.u8(new_off)]
            if child is not None:
                stack.append((child, new_off + 1))
        return None


# ---------------------------------------------------------------------------
# 解析器主体 (对应原项目 ESymbol)
# ---------------------------------------------------------------------------

def _is_menu_id(cid):
    return (cid & 0x0F000000) == 0x06000000 and (cid & 0xF0000000) == 0x20000000


def _get_data_type_name(state, type_id):
    """类型ID -> 支持库数据类型名 (对应原项目 getControlTypeName):
    高两位为 0x80000000 或 0x40000000 的不是控件类型, 其余按 库索引<<16|类型索引 解析"""
    if type_id == 0:
        return ""
    flags = type_id & 0xC0000000
    if flags == 0x80000000 or flags == 0x40000000:
        return ""
    lib_index = (type_id >> 0x10) - 1
    type_index = (type_id & 0xFFFF) - 1
    libs = state["libs"]
    if lib_index < 0 or lib_index >= len(libs):
        return ""
    types = libs[lib_index]["dataTypes"]
    if type_index < 0 or type_index >= len(types):
        return ""
    return types[type_index]["name"]


def _event_name(state, control, event_index):
    type_name = control.get("controlTypeName", "")
    cls = _CONTROL_EVENT_MAP.get(type_name)
    table = {}
    if cls:
        table = EVENT_TABLES["controls"].get(cls, {})
    if str(event_index) in table:
        return table[str(event_index)]
    if str(event_index) in EVENT_TABLES["common"]:
        return EVENT_TABLES["common"][str(event_index)]
    return "事件%d" % event_index


def _property_name(control, prop_index):
    type_name = control.get("controlTypeName", "")
    cls = _CONTROL_EVENT_MAP.get(type_name)
    table = EVENT_TABLES.get("properties", {}).get(cls, {}) if cls else {}
    if str(prop_index) in table:
        return table[str(prop_index)]
    return "属性%d" % prop_index


def _set_func_name(ea, name):
    if name:
        ida_name.set_name(ea, name, ida_name.SN_NOWARN | ida_name.SN_FORCE)


def _apply_cdecl(ea, decl):
    """尽力为函数应用原型 (IDA 9.x 类型 API, 失败则忽略)"""
    try:
        import ida_typeinf
        tif = ida_typeinf.tinfo_t()
        idati = ida_typeinf.get_idati()
        flags = ida_typeinf.PT_SIL
        if ida_typeinf.parse_decl(tif, idati, decl, flags) is None:
            return False
        return ida_typeinf.apply_tinfo(ea, tif, ida_typeinf.TINFO_DEFINIT)
    except Exception:
        return False


def analyze(log=None):
    """主入口: 完整解析当前 IDB 中的易语言程序"""
    log = log or (lambda s: ida_kernwin.msg("[E-MCP] %s\n" % s))
    state = STATE
    state.update(analyzed=False, error=None, info={}, libs=[], controls=[],
                 imports=[], functions={}, krnl={}, vtables=[], unmatched_cmds=[])

    mem = Memory()
    state["info"]["imageBase"] = hex(mem.base)

    # 1. 探测静态编译特征 (对应原项目 initEArchitectureType)
    magic = bytes.fromhex("506489250000000081ECAC010000535657")
    pos = mem.buf.find(magic)
    if pos == -1:
        state["error"] = "未找到易语言静态编译特征, 这可能不是易语言静态编译程序"
        log(state["error"])
        return state
    magic_addr = mem.lin(pos)
    ehead_addr = mem.u32(pos + 0x26)
    ehead_off = mem.off(ehead_addr)
    if ehead_off < 0 or mem.u32(ehead_off) != 3:
        state["error"] = "易语言头校验失败 (magic != 3)"
        log(state["error"])
        return state

    user_code_start = mem.u32(ehead_off + 3 * 4)
    user_code_end = ehead_addr
    lp_estring = mem.u32(ehead_off + 4 * 4)
    dw_estring_size = mem.u32(ehead_off + 5 * 4)
    lp_ewindow = mem.u32(ehead_off + 6 * 4)
    dw_ewindow_size = mem.u32(ehead_off + 7 * 4)
    dw_lib_num = mem.u32(ehead_off + 8 * 4)
    lp_lib_entry = mem.u32(ehead_off + 9 * 4)
    dw_api_count = mem.u32(ehead_off + 10 * 4)
    lp_module_name = mem.u32(ehead_off + 11 * 4)
    lp_api_name = mem.u32(ehead_off + 12 * 4)

    state["info"]["userCodeStart"] = hex(user_code_start)
    state["info"]["userCodeEnd"] = hex(user_code_end)
    state["info"]["eHead"] = hex(ehead_addr)

    # 2. 批量创建函数 (55 8B EC)
    log("扫描用户函数...")
    func_count = 0
    start = mem.off(user_code_start)
    end = mem.off(user_code_end)
    prolog = b"\x55\x8b\xec"
    pos2 = start
    while True:
        pos2 = mem.buf.find(prolog, pos2, end)
        if pos2 == -1:
            break
        ea = mem.lin(pos2)
        if ida_funcs.add_func(ea):
            func_count += 1
        pos2 += 3
    state["info"]["createdFunctions"] = func_count
    ida_auto.auto_wait()

    # 3. 解析支持库
    log("解析易语言支持库...")
    off = mem.off(lp_lib_entry)
    for _ in range(dw_lib_num):
        if off < 0:
            break
        lib_ptr = mem.u32(off)
        off += 4
        lo = mem.off(lib_ptr)
        if lo < 0 or mem.u32(lo) != 0x1312D65:
            continue
        lib = {
            "name": mem.cstr(mem.off(mem.u32(lo + 9 * 4))) if mem.off(mem.u32(lo + 9 * 4)) >= 0 else "",
            "guid": mem.cstr(mem.off(mem.u32(lo + 1 * 4))) if mem.off(mem.u32(lo + 1 * 4)) >= 0 else "",
            "majorVersion": mem.i32(lo + 2 * 4),
            "minorVersion": mem.i32(lo + 3 * 4),
            "dataTypes": [],
            "cmdCount": mem.i32(lo + 25 * 4),
            "cmds": [],
        }
        # 数据类型 (注意: 无名类型也要占位, 否则 typeId 索引会错位)
        dt_count = mem.i32(lo + 21 * 4)
        dt = mem.off(mem.u32(lo + 22 * 4))
        for di in range(dt_count):
            if dt < 0:
                break
            name_ptr = mem.u32(dt)
            name = mem.cstr(mem.off(name_ptr)) if name_ptr and mem.off(name_ptr) >= 0 else ""
            type_id = ((len(state["libs"]) + 1) << 0x10) + (di + 1)
            lib["dataTypes"].append({"name": name, "typeId": hex(type_id) if name else None})
            dt += 56  # sizeof(LIB_DATA_TYPE_INFO) = 14 dwords
        # 命令表
        cmd_count = lib["cmdCount"]
        cf = mem.off(mem.u32(lo + 27 * 4))
        for ci in range(max(0, cmd_count)):
            if cf < 0:
                break
            lib["cmds"].append(mem.u32(cf))
            cf += 4
        state["libs"].append(lib)

    # 4. esig 特征码识别库函数
    log("用特征码识别支持库函数...")
    sig_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "esig")
    for lib_index, lib in enumerate(state["libs"]):
        if not lib["cmdCount"] or not lib["cmds"]:
            continue
        tree = TrieTree()
        sig_path = os.path.join(sig_dir, lib["name"] + ".esig")
        if not tree.load_sig(sig_path):
            log("缺少特征文件: %s" % sig_path)
            continue
        for ci, func_addr in enumerate(lib["cmds"]):
            fo = mem.off(func_addr)
            state["functions"][func_addr] = {"name": None, "type": "KrnlLibFunc", "lib": lib["name"]}
            if fo >= 0:
                ida_funcs.add_func(mem.lin(fo))
            name = tree.match_func(mem, fo) if fo >= 0 else None
            if name:
                lib["cmds"][ci] = {"addr": hex(func_addr), "name": name}
                state["functions"][func_addr]["name"] = name
                _set_func_name(func_addr, name)
            else:
                lib["cmds"][ci] = {"addr": hex(func_addr), "name": None}
                state["unmatched_cmds"].append({"lib": lib["name"], "addr": hex(func_addr)})
    ida_auto.auto_wait()

    # 5. 基础命令特征
    log("识别易语言基础命令...")
    basic = TrieTree()
    basic_path = os.path.join(sig_dir, "易语言基础命令.esig")
    if basic.load_sig(basic_path):
        for idx in range(ida_funcs.get_func_qty()):
            fn = ida_funcs.getn_func(idx)
            if fn is None or fn.start_ea >= user_code_end:
                continue
            fo = mem.off(fn.start_ea)
            if fo < 0:
                continue
            name = basic.match_func(mem, fo)
            if name:
                _set_func_name(fn.start_ea, name)
                state["functions"][fn.start_ea] = {"name": name, "type": "BasicFunc"}
                if name == "文本相加":
                    _apply_cdecl(fn.start_ea, "char* __usercall strcat@<eax>(int argCount@<ecx>, ...);")
                elif name == "连续省略参数":
                    _apply_cdecl(fn.start_ea, "void __usercall pushDefaultParam(int argCount@<ebx>);")

    # 6. krnln 跳转接口 (对应原项目 loadKrnlInterface)
    log("定位核心支持库跳转表...")
    krnl_entry = lp_estring if lp_estring else lp_ewindow
    ok = _load_krnl_interface(state, mem, krnl_entry)
    if not ok:
        log("警告: krnln 跳转表定位失败")

    # 7. 类虚表扫描 (对应原项目 scanEClassTable)
    log("扫描易语言类虚表...")
    _scan_class_tables(state, mem, user_code_start, user_code_end)

    # 8. 窗口/控件/事件资源
    if lp_ewindow and dw_ewindow_size > 4:
        log("解析易语言控件资源...")
        _load_gui_resource(state, mem, lp_ewindow, dw_ewindow_size, user_code_start)

    # 9. 用户导入表
    if dw_api_count:
        log("解析用户导入表...")
        _load_user_imports(state, mem, dw_api_count, lp_module_name, lp_api_name)

    # 10. 命名事件函数 (对应原项目 setGuiEventName)
    for ctrl in state["controls"]:
        for ev in ctrl["events"]:
            name = "_%s_%s" % (ctrl["name"], _event_name(state, ctrl, ev["index"]))
            _set_func_name(ev["addr"], name)
            state["functions"][ev["addr"]] = {"name": name, "type": "EventHandler",
                                              "control": ctrl["name"]}

    state["info"]["windowCount"] = len([c for c in state["controls"] if c["isWindow"]])
    state["info"]["controlCount"] = len(state["controls"])
    state["info"]["eventCount"] = sum(len(c["events"]) for c in state["controls"])
    state["info"]["libCount"] = len(state["libs"])
    state["info"]["importCount"] = len(state["imports"])
    state["analyzed"] = True

    # 控件索引 (int 键, 供 Hex-Rays 修正按 windowId/controlId 查找)
    global CONTROL_INDEX
    CONTROL_INDEX = {}
    for c in state["controls"]:
        try:
            CONTROL_INDEX[(int(c["windowId"], 0), int(c["controlId"], 0))] = c
        except (KeyError, ValueError):
            pass

    # 安装 Hex-Rays 反编译修正钩子 (纯 Python, 无 SDK)
    try:
        import e_mcp_hexrays
        e_mcp_hexrays.install_fixer(state)
    except ImportError:
        log("提示: e_mcp_hexrays 未安装, 跳过反编译修正")

    log("解析完成: %s" % json.dumps(state["info"], ensure_ascii=False))
    return state


def _load_krnl_interface(state, mem, krnl_entry):
    if not krnl_entry:
        return False
    jmp_other = mem.search(b"\xff\x25" + struct.pack("<I", krnl_entry - 4))
    if jmp_other == -1:
        return False
    jmp = {k: None for k in KRNL_JMP_NAMES}
    jmp["Jmp_MOtherHelp"] = mem.lin(jmp_other)

    call_tbl_off = mem.off(krnl_entry) - len(KRNL_CALL_FIELDS) * 4
    calls = {}
    if call_tbl_off >= 0:
        for i, field in enumerate(KRNL_CALL_FIELDS):
            calls[field] = mem.u32(call_tbl_off + i * 4)

    def register(call_addr, set_addr):
        for i, field in enumerate(KRNL_CALL_FIELDS):
            if calls.get(field) == call_addr:
                jmp[field.replace("krnl_", "Jmp_")] = set_addr
                return True
        return False

    # 向上扫描连续的 jmp [mem]
    addr = jmp_other
    while True:
        insn = ida_ua.insn_t()
        prev = ida_ua.decode_prev_insn(insn, mem.lin(addr))
        if prev == ida_idaapi.BADADDR or insn.itype != ida_allins.NN_jmpni \
                or insn.ops[0].type != ida_ua.o_mem:
            break
        target_off = mem.off(insn.ops[0].addr)
        if target_off < 0:
            break
        if not register(mem.u32(target_off), mem.lin(addr)):
            break
        addr = mem.off(prev)

    # 向下扫描
    addr = jmp_other
    while True:
        insn = ida_ua.insn_t()
        length = ida_ua.decode_insn(insn, mem.lin(addr))
        if not length:
            return False
        if insn.itype != ida_allins.NN_jmpni or insn.ops[0].type != ida_ua.o_mem:
            break
        target_off = mem.off(insn.ops[0].addr)
        if target_off < 0:
            break
        register(mem.u32(target_off), mem.lin(addr))
        addr += insn.size

    for field, name in KRNL_JMP_NAMES.items():
        ea = jmp.get(field)
        if ea:
            _set_func_name(ea, name)
            state["functions"][ea] = {"name": name, "type": field}
    state["krnl"] = {k: (hex(v) if v else None) for k, v in jmp.items()}

    decls = {
        "Jmp_MCallDllCmd": "krnlRet __usercall CallDllCmd@<eax:edx>(unsigned int index@<eax>,...);",
        "Jmp_MCallLibCmd": "krnlRet __usercall CallLibCmd@<eax:edx>(unsigned int libFunc@<ebx>, int argCount, ...);",
        "Jmp_MCallKrnlLibCmd": "krnlRet __usercall CallKrnlLibCmd@<eax:edx>(unsigned int libFunc@<ebx>, int argCount, ...);",
        "Jmp_MReadProperty": "krnlRet __usercall CallReadProperty@<eax:edx>(unsigned int,unsigned int,unsigned int,unsigned int);",
        "Jmp_MWriteProperty": "void __cdecl CallWriteProperty(unsigned int windowId,unsigned int controlId,unsigned int nPropertyIndex,unsigned int nDataSize,UNIT_PROPERTY_VALUE,unsigned int ppszTipText);",
        "Jmp_MFree": "void __cdecl CallFree(LPVOID lpMem);",
        "Jmp_MMalloc": "LPVOID __cdecl malloc(SIZE_T dwBytes);",
        "Jmp_MOtherHelp": "krnlRet __usercall CallOtherHelp@<eax:edx>(unsigned int index@<eax>,...);",
    }
    for field, decl in decls.items():
        if jmp.get(field):
            _apply_cdecl(jmp[field], decl)
    return True


def _scan_class_tables(state, mem, user_code_start, user_code_end):
    """扫描 mov dword ptr [ebx], imm32 形式的类虚表引用"""
    hi = (user_code_start >> 0x18) & 0xFF
    # C7 03 <dword> 其中 dword 高字节 == hi
    guess_set = set()
    pos = mem.off(user_code_start)
    end = mem.off(user_code_end)
    pat = re.compile(re.escape(b"\xc7\x03") + b"(.{4})", re.S)
    for m in pat.finditer(mem.buf, pos, end):
        val = struct.unpack("<I", m.group(1))[0]
        if (val >> 0x18) == hi:
            guess = mem.u32(m.start() + 2)
            if guess < user_code_start:
                continue
            vt_off = mem.off(guess)
            if vt_off < 0:
                continue
            # 校验虚表中第2项开头为 50 68 (push eax; push imm)
            try:
                copy_head = mem.u16(mem.off(mem.u32(vt_off + 4)))
            except Exception:
                continue
            if copy_head == 0x6850:
                guess_set.add(guess)
    for vt in sorted(guess_set):
        entries = []
        off = mem.off(vt)
        first = mem.u32(off)
        second = mem.u32(off + 4)
        entries.extend([first, second])
        ptr = vt + 8
        while True:
            if ptr in guess_set:
                break
            f = mem.u32(mem.off(ptr)) if mem.off(ptr) >= 0 else 0
            if f <= user_code_start or f >= user_code_end:
                break
            entries.append(f)
            ptr += 4
        state["vtables"].append({"addr": hex(vt), "functions": [hex(f) for f in entries]})
        name = "vtable_%08X" % vt
        ida_name.set_name(vt, name, ida_name.SN_NOWARN | ida_name.SN_FORCE)


def _load_gui_resource(state, mem, gui_start, info_size, user_code_start):
    base = mem.off(gui_start)
    if base < 0 or base + info_size > len(mem.buf):
        return
    buf = mem.buf
    p = base
    total_windows = struct.unpack_from("<I", buf, p)[0] >> 3
    p += 4
    window_ids = []
    for _ in range(total_windows):
        window_ids.append(struct.unpack_from("<I", buf, p)[0])
        p += 4
    p += 4 * total_windows  # 编译器遗留值

    for w in range(total_windows):
        wp = p
        wp += 4 + 4  # 两个未知字段
        wp += 8      # 两个空 CString
        control_count = struct.unpack_from("<I", buf, wp)[0]
        wp += 4
        control_size = struct.unpack_from("<I", buf, wp)[0]
        wp += 4
        q = wp
        control_ids = [struct.unpack_from("<I", buf, q + 4 * i)[0] for i in range(control_count)]
        q += 4 * control_count
        control_offsets = [struct.unpack_from("<I", buf, q + 4 * i)[0] for i in range(control_count)]
        q += 4 * control_count

        for i in range(control_count):
            cp = q + control_offsets[i]
            control_size_i = struct.unpack_from("<i", buf, cp)[0]
            cp += 4
            type_id = struct.unpack_from("<I", buf, cp)[0]
            cp += 4
            cp += 20  # 保留字节

            cid = control_ids[i]
            ctrl = {
                "windowId": hex(window_ids[w]),
                "controlId": hex(cid),
                "controlTypeId": hex(type_id),
                "controlTypeName": _get_data_type_name(state, type_id),
                "isWindow": type_id == 0x10001,
                "name": "",
                "events": [],
                "propertyAddr": hex(mem.lin(cp)),
                "propertySize": control_size_i,
            }

            def read_cstr(pos):
                end = buf.find(b"\x00", pos, pos + 1024)
                if end == -1:
                    end = pos + 1024
                return buf[pos:end].decode("gbk", errors="replace"), end + 1

            if type_id == 0x10001:
                name, cp = read_cstr(cp)
                ctrl["name"] = name or ("窗口0x%08X" % window_ids[w])
                cp = _parse_control_basic_property(buf, cp, ctrl, user_code_start)
            elif _is_menu_id(cid):
                cp += 14
                name, _ = read_cstr(cp)
                ctrl["name"] = name
            else:
                name, cp = read_cstr(cp)
                ctrl["name"] = name
                cp = _parse_control_basic_property(buf, cp, ctrl, user_code_start)

            state["controls"].append(ctrl)
        p = wp + control_size


def _parse_control_basic_property(buf, p, ctrl, user_code_start):
    def skip_cstr(pos):
        end = buf.find(b"\x00", pos, pos + 1024)
        return (end + 1) if end != -1 else pos + 1024

    p = skip_cstr(p)          # 无用字符串
    p += 4                    # 未知
    p += 16                   # left/top/width/height
    p += 4                    # hCURSOR
    ctrl["parentId"] = hex(struct.unpack_from("<I", buf, p)[0])
    p += 4
    child_count = struct.unpack_from("<I", buf, p)[0]
    p += 4 + 4 * child_count
    offset2 = struct.unpack_from("<I", buf, p)[0]
    p += offset2 + 4
    p = skip_cstr(p)          # 标记
    p += 12                   # 未知
    event_count = struct.unpack_from("<i", buf, p)[0]
    p += 4
    for _ in range(event_count):
        idx = struct.unpack_from("<i", buf, p)[0]
        addr = struct.unpack_from("<I", buf, p + 4)[0] + user_code_start
        ctrl["events"].append({"index": idx, "addr": addr})
        p += 8
    return p


def _load_user_imports(state, mem, api_count, module_name, api_name):
    mo = mem.off(module_name)
    ao = mem.off(api_name)
    for _ in range(api_count):
        if mo < 0 or ao < 0:
            break
        lib_name = mem.cstr(mem.off(mem.u32(mo))) if mem.off(mem.u32(mo)) >= 0 else ""
        api_name_str = mem.cstr(mem.off(mem.u32(ao))) if mem.off(mem.u32(ao)) >= 0 else ""
        mo += 4
        ao += 4
        short_lib = lib_name.rsplit(".", 1)[0] if "." in lib_name else lib_name
        full = (short_lib + "." + api_name_str) if short_lib else api_name_str
        state["imports"].append({"lib": lib_name, "api": api_name_str, "full": full})
