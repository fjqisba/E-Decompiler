# -*- coding: utf-8 -*-
"""
test_e_mcp_dbg.py — 在普通 Python 下用 mock 验证 e_mcp_dbg 的核心逻辑
(不依赖 IDA, 只覆盖与 IDA API 交互的部分用桩替换)

用法: python test_e_mcp_dbg.py
"""
import sys
import types

# ---------------------------------------------------------------------------
# IDA API 桩
# ---------------------------------------------------------------------------

IMAGE_LO, IMAGE_HI = 0x400000, 0x500000
USER_START, USER_END = 0x401004, 0x4F17C0

MEM = {
    # 栈: esp=0x1FF000
    0x1FEFF0: bytes.fromhex("34124000"   # [0x1FEFF0] = 0x401234 (返回地址, 映像内)
                            "89674500"   # [0x1FEFF4] = 0x456789 (文本指针参数)
                            "d2040000"   # [0x1FEFF8] = 1234
                            ),
    # 文本 "你好世界" GBK + 长度前缀8 + \0
    0x456785: bytes.fromhex("08000000") + "你好世界".encode("gbk") + b"\x00",
    # MWriteProperty 栈: [esp]=retaddr, [esp+4]=windowId=1, [esp+8]=controlId=2, [esp+C]=propIndex=1
    0x1FEFC0: bytes.fromhex("34124000" "01000000" "02000000" "01000000"),
}

# 简化: 直接按 (块起始, 数据) 精确读取
def mem_read2(ea, size):
    out = bytearray()
    for i in range(size):
        a = ea + i
        for start, data in MEM.items():
            if start <= a < start + len(data):
                out.append(data[a - start])
                break
        else:
            return None if i == 0 else bytes(out)
    return bytes(out)


ida_bytes = types.ModuleType("ida_bytes")
ida_bytes.get_bytes = mem_read2

ida_dbg = types.ModuleType("ida_dbg")
ida_dbg.BPT_ENABLED = 1
ida_dbg.BPT_SOFT = 0
ida_dbg.BPTS = []


def _add_bpt(ea, size=0, typ=0):
    ida_dbg.BPTS.append(ea)
    return True


ida_dbg.add_bpt = _add_bpt
ida_dbg.run_to = lambda ea: True
ida_dbg.get_bpt_qty = lambda: len(ida_dbg.BPTS)


class _Bpt:
    def __init__(self, ea):
        self.ea = ea
        self.flags = 1
        self.condition = None


ida_dbg.bpt_t = _Bpt


def _getn_bpt(i, bpt):
    if i < len(ida_dbg.BPTS):
        bpt.ea = ida_dbg.BPTS[i]
        return True
    return False


ida_dbg.getn_bpt = _getn_bpt

REGS = {"EAX": 1, "EBX": 0x10002, "ECX": 0, "EDX": 0, "ESI": 0, "EDI": 0,
        "EBP": 0x1FF100, "ESP": 0x1FEFF0, "EIP": 0x48FFF0}

ida_dbg.get_ip_val = lambda: REGS["EIP"]
ida_dbg.get_current_thread = lambda: 0x1234


class _RegVal:
    def __init__(self, v):
        self.v = v

    def pyval(self, dtype):
        return self.v


class _RegInfo:
    def __init__(self, name):
        self.name = name
        self.dtype = None


class _Dbg:
    _names = list(REGS)

    def regs(self, i):
        if i < len(self._names):
            return _RegInfo(self._names[i])
        raise IndexError


ida_dbg.get_reg_vals = lambda tid: [_RegVal(REGS[n]) for n in REGS]

ida_idd = types.ModuleType("ida_idd")
ida_idd.get_dbg = lambda: _Dbg()


class _Frame:
    def __init__(self, callea):
        self.callea = callea


class _Trace:
    def __init__(self, frames=None):
        self._f = frames or []

    def __iter__(self):
        return iter(self._f)


ida_idd.call_stack_t = _Trace


def _collect(tid, trace):
    trace._f = [_Frame(0x48FFF0), _Frame(0x404FFC), _Frame(0x7C80234B)]
    return True


ida_dbg.collect_stack_trace = _collect

ida_funcs = types.ModuleType("ida_funcs")
_FUNCS = {0x404FFC: 0x404FFC, 0x405000: 0x404FFC, 0x48FFF0: 0x48FFF0,
          0x48FFD0: 0x48FFD0, 0x48FFC8: 0x48FFC8}


class _Func:
    def __init__(self, start):
        self.start_ea = start


ida_funcs.get_func = lambda ea: _Func(_FUNCS[ea]) if ea in _FUNCS else None

ida_idaapi = types.ModuleType("ida_idaapi")
ida_name = types.ModuleType("ida_name")
ida_name.get_name = lambda ea: {0x7C80234B: "MessageBoxA"}.get(ea, "")

ida_segment = types.ModuleType("ida_segment")
ida_segment.getseg = lambda ea: (IMAGE_LO <= ea < IMAGE_HI)


class _PropIdx:
    """属性索引表: (类型类, 索引) -> 名字"""
    DATA = {"CKrnl_EditBox": {"1": "内容", "2": "标题"}}


for name, mod in [("ida_bytes", ida_bytes), ("ida_dbg", ida_dbg),
                  ("ida_idd", ida_idd), ("ida_funcs", ida_funcs),
                  ("ida_idaapi", ida_idaapi), ("ida_name", ida_name),
                  ("ida_segment", ida_segment)]:
    sys.modules[name] = mod

# ---------------------------------------------------------------------------
# e_mcp_core 桩 (使用 rxxb 实测数据形态)
# ---------------------------------------------------------------------------

IMPORTS = [
    {"lib": "", "api": "EnumWindows", "full": "EnumWindows"},
    {"lib": "", "api": "MessageBoxA", "full": "MessageBoxA"},  # 简化, 测试时索引 1
]

core = types.ModuleType("e_mcp_core")
core.KRNL_JMP_NAMES = {
    "Jmp_MReportError": "错误回调", "Jmp_MCallDllCmd": "DLL命令",
    "Jmp_MCallLibCmd": "三方支持库命令", "Jmp_MCallKrnlLibCmd": "核心支持库命令",
    "Jmp_MReadProperty": "读取组件属性", "Jmp_MWriteProperty": "设置组件属性",
    "Jmp_MMalloc": "分配内存", "Jmp_MRealloc": "重新分配内存",
    "Jmp_MFree": "释放内存", "Jmp_MExitProcess": "结束",
    "Jmp_MMessageLoop": "窗口消息循环", "Jmp_MLoadBeginWin": "载入启动窗口",
    "Jmp_MOtherHelp": "辅助函数",
}
core.CONTROL_INDEX = {(1, 2): {"name": "编辑框1", "controlTypeName": "编辑框",
                               "windowId": "0x1", "controlId": "0x2"}}
core.EVENT_TABLES = {"common": {}, "controls": {}, "properties": {}}


def _property_name(control, prop_index):
    table = {"编辑框": {"1": "内容"}}
    cls = {"编辑框": "编辑框"}.get(control.get("controlTypeName"))
    t = table.get(control.get("controlTypeName"), {})
    return t.get(str(prop_index), "属性%d" % prop_index)


core._property_name = _property_name
core.STATE = {
    "analyzed": True, "error": None,
    "info": {"userCodeStart": hex(USER_START), "userCodeEnd": hex(USER_END),
             "imageBase": hex(IMAGE_LO)},
    "libs": [{"name": "krnln", "cmds": [{"addr": hex(0x471000), "name": "取窗口句柄"},
                                        {"addr": hex(0x472000), "name": "信息框"}]}],
    "controls": [], "imports": IMPORTS,
    "functions": {
        0x404FFC: {"name": "_保存_被单击", "type": "EventHandler", "control": "保存"},
        0x405915: {"name": "_自动监控_被单击", "type": "EventHandler", "control": "自动监控"},
        0x471000: {"name": "取窗口句柄", "type": "KrnlLibFunc", "lib": "krnln"},
        0x472000: {"name": "信息框", "type": "KrnlLibFunc", "lib": "krnln"},
        0x48FFF0: {"name": "DLL命令", "type": "Jmp_MCallDllCmd"},
        0x48FFD0: {"name": "分配内存", "type": "Jmp_MMalloc"},
        0x48FFC8: {"name": "设置组件属性", "type": "Jmp_MWriteProperty"},
    },
    "krnl": {"Jmp_MCallDllCmd": hex(0x48FFF0), "Jmp_MMalloc": hex(0x48FFD0),
             "Jmp_MWriteProperty": hex(0x48FFC8), "Jmp_MReportError": None},
    "vtables": [], "unmatched_cmds": [],
}
sys.modules["e_mcp_core"] = core

# ---------------------------------------------------------------------------
# 被测模块 + 断言
# ---------------------------------------------------------------------------

sys.path.insert(0, r"D:\易语言的MCP\e-mcp")
import e_mcp_dbg as d

fails = []


def check(cond, msg):
    print(("PASS " if cond else "FAIL ") + msg)
    if not cond:
        fails.append(msg)


# 1. krnln 语义名精确匹配
hits = d.resolve_targets("DLL命令")
check(len(hits) == 1 and hits[0]["address"] == hex(0x48FFF0), "resolve DLL命令 -> 0x48fff0")

# 2. 事件子串
hits = d.resolve_targets("保存")
check(len(hits) == 1 and hits[0]["name"] == "_保存_被单击", "resolve 保存 -> _保存_被单击")

# 3. 多匹配
hits = d.resolve_targets("被单击")
check(len(hits) == 2, "resolve 被单击 -> 2 个事件函数")

# 4. 库函数子串
hits = d.resolve_targets("信息框")
check(len(hits) == 1 and hits[0]["kind"] == "KrnlLibFunc", "resolve 信息框 -> 库函数")

# 5. context: 断在 DLL命令 thunk, eax=1 -> MessageBoxA 候选
REGS["EIP"] = 0x48FFF0
ctx = d.context()
check(ctx["currentFunction"]["name"] == "DLL命令", "context: 识别当前函数")
check(ctx["krnlHit"] and ctx["krnlHit"]["apiCandidates"][1]["full"] == "MessageBoxA",
      "context: DLL命令现场给出 API 候选")
check(ctx["callStack"][1]["eFunction"] == "_保存_被单击", "context: 调用栈映射中文函数名")
check(ctx["callStack"][2].get("symbol") == "MessageBoxA", "context: 非E帧给符号名")

# 6. context: 断在 分配内存
REGS["EIP"] = 0x48FFD0
ctx = d.context()
check(ctx["krnlHit"]["meaning"].startswith("正在分配内存"), "context: malloc 现场解释")

# 7. context: 断在 写属性, 栈参数 -> 控件/属性
REGS["EIP"] = 0x48FFC8
REGS["ESP"] = 0x1FEFC0
ctx = d.context()
check(ctx["krnlHit"].get("target", {}).get("control") == "编辑框1"
      and ctx["krnlHit"]["target"]["property"] == "内容",
      "context: 写属性现场给出 控件+属性名")

# 8. dump_args: 断点在函数入口(未压栈) -> esp+4, args[0]=第一个参数
REGS["EIP"] = 0x404FFC
REGS["ESP"] = 0x1FEFF0
REGS["EBP"] = 0x1FF100
args = d.dump_args(4)
check(args["argsBase"].startswith("esp+4"), "args: 入口断点用 esp+4")
check(args["args"][0]["value"] == hex(0x456789), "args: 第一个参数正确")
check("文本指针" in args["args"][0]["meaning"] and "你好世界" in args["args"][0]["meaning"],
      "args: 文本指针解释 -> 你好世界")

# 9. read_text: 长度前缀 + GBK
t = d.read_text(hex(0x456789), 64)
check(t["text"] == "你好世界" and t["lengthPrefixHint"] == 8, "read_text: GBK+长度前缀")

# 10. run_to: 多匹配拒绝, 单匹配执行
r = d.run_to_target("被单击")
check("error" in r, "run_to: 多匹配报错不执行")
r = d.run_to_target("DLL命令")
check(r["ranTo"]["address"] == hex(0x48FFF0), "run_to: 单匹配执行")

# 11. targets: krnln 只列已定位的
tg = d.get_targets("")
check(len(tg["krnlCalls"]) == 3, "targets: 只列出已定位的 krnln 入口")
check(any(e["name"] == "_保存_被单击" for e in tg["events"]), "targets: 事件列表")

print()
if fails:
    print("%d 项失败:" % len(fails))
    for f in fails:
        print(" -", f)
    sys.exit(1)
print("全部通过")
