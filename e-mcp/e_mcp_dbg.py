# -*- coding: utf-8 -*-
"""
e_mcp_dbg.py — 易语言程序动态调试语义层 (IDA 9.4 IDAPython)

在 ida-pro-mcp 通用调试器工具 (dbg_start_process / dbg_continue_process /
dbg_step_into / dbg_step_over / dbg_get_registers / decompile_function ...)
之上叠加易语言语义:

  * 按中文名下断点: 事件处理函数(_保存_被单击)、krnln 语义入口
    (DLL命令/错误回调/读写属性/分配内存...)、特征码识别出的支持库函数
  * 暂停位置解读: 当前 E 函数、关键寄存器、映射为中文函数名的调用栈
  * krnln 调用现场解释: 断在 DLL命令 时报告正在调用哪个 API, 断在
    核心支持库命令/三方支持库命令 时报告命令名, 断在写属性时报告
    目标控件与属性名
  * 易语言数据解释: 文本型指针(GBK, 指针前4字节通常为长度)读值

约定:
  * 本模块所有函数都应在 IDA 主线程调用 (extension 层用 execute_sync 调度)
  * 只依赖 e_mcp_core 的解析结果 (STATE / CONTROL_INDEX / KRNL_JMP_NAMES),
    调用前必须已经 elang_analyze()
"""

import struct

import ida_bytes
import ida_dbg
import ida_funcs
import ida_idd
import ida_idaapi
import ida_name
import ida_segment


# ---------------------------------------------------------------------------
# 基础设施
# ---------------------------------------------------------------------------

def _core():
    import e_mcp_core
    return e_mcp_core


def require_analyzed():
    state = _core().STATE
    if not state.get("analyzed"):
        raise RuntimeError("当前 IDB 尚未执行易语言解析, 请先调用 elang_analyze")


def _parse_hex(s):
    return int(str(s), 16)


def dbg_ip():
    """当前暂停位置的 EIP, 未暂停返回 None"""
    try:
        return ida_dbg.get_ip_val()
    except Exception:
        return None


def require_suspended():
    ip = dbg_ip()
    if ip is None:
        raise RuntimeError(
            "调试器未处于暂停状态, 请先 dbg_start_process 启动调试并等待断点命中")
    return ip


def read_mem(ea, size):
    """读调试目标进程内存, 失败返回 None"""
    try:
        return ida_bytes.get_bytes(ea, size)
    except Exception:
        return None


def read_mem_u32(ea):
    data = read_mem(ea, 4)
    if not data or len(data) < 4:
        return None
    return struct.unpack("<I", data)[0]


# ---------------------------------------------------------------------------
# 目标解析: 名字 -> 断点地址
# ---------------------------------------------------------------------------

def _krnl_targets():
    state = _core().STATE
    out = []
    for field, cname in _core().KRNL_JMP_NAMES.items():
        addr = state["krnl"].get(field)
        if addr:
            out.append({"name": cname, "field": field,
                        "address": addr, "kind": "krnlCall"})
    return out


def _event_targets(filt=None):
    state = _core().STATE
    out = []
    for addr, info in sorted(state["functions"].items()):
        if info.get("type") != "EventHandler":
            continue
        name = info.get("name") or ""
        if filt and filt not in name:
            continue
        out.append({"name": name, "control": info.get("control", ""),
                    "address": hex(addr), "kind": "eventHandler"})
    return out


def get_targets(filt=""):
    """列出可断点的易语言目标"""
    require_analyzed()
    filt = (filt or "").strip()
    krnl = [k for k in _krnl_targets() if not filt or filt in k["name"] or filt in k["field"]]
    events = _event_targets(filt or None)
    named = sum(1 for i in _core().STATE["functions"].values() if i.get("name"))
    return {
        "krnlCalls": krnl,
        "events": events,
        "namedFunctionCount": named,
        "hint": "断点目标可用 elang_dbg_break(名称子串) 设置; 支持库函数/基础函数"
                "可先用 elang_find_function 查到地址后用 dbg_set_breakpoint",
    }


def resolve_targets(name):
    """按名称解析断点目标。

    匹配顺序:
      1. krnln 语义名精确匹配 (DLL命令 / 错误回调 / Jmp_MCallDllCmd / MCallDllCmd)
      2. 所有已命名易语言函数(事件/库函数/基础函数)的名字子串匹配
    """
    require_analyzed()
    key = (name or "").strip()
    if not key:
        return []
    state = _core().STATE

    hits = []
    # 1. krnln 语义名精确匹配
    for field, cname in _core().KRNL_JMP_NAMES.items():
        addr = state["krnl"].get(field)
        if not addr:
            continue
        short = field.replace("Jmp_", "")
        if key in (cname, field, short):
            hits.append({"name": cname, "address": addr, "kind": "krnlCall",
                         "field": field})
    if hits:
        return hits

    # 2. 命名函数子串匹配
    seen = set()
    for addr, info in sorted(state["functions"].items()):
        fname = info.get("name")
        if not fname or key not in fname or addr in seen:
            continue
        seen.add(addr)
        hits.append({"name": fname, "address": hex(addr),
                     "kind": info.get("type", "function"),
                     "control": info.get("control", "")})
    return hits


def add_breakpoints(hits, max_set=8):
    """为解析出的目标设置断点, 返回 (已设置的, 超限未设置的)"""
    if len(hits) > max_set:
        return [], hits
    done = []
    for h in hits:
        ea = _parse_hex(h["address"])
        if ida_dbg.add_bpt(ea, 0, ida_dbg.BPT_SOFT):
            done.append(h)
    return done, []


def list_breakpoints():
    out = []
    for i in range(ida_dbg.get_bpt_qty()):
        bpt = ida_dbg.bpt_t()
        if ida_dbg.getn_bpt(i, bpt):
            out.append({"ea": hex(bpt.ea),
                        "enabled": bool(bpt.flags & ida_dbg.BPT_ENABLED),
                        "eFunction": (find_func_by_addr(bpt.ea) or {}).get("name")})
    return out


# ---------------------------------------------------------------------------
# 暂停现场解读
# ---------------------------------------------------------------------------

def get_key_registers():
    """提取关键寄存器 (EAX/EBX/ECX/EDX/ESI/EDI/EBP/ESP/EIP), 未暂停返回 {}"""
    try:
        dbg = ida_idd.get_dbg()
        if dbg is None:
            return {}
        tid = ida_dbg.get_current_thread()
        vals = ida_dbg.get_reg_vals(tid)
    except Exception:
        return {}
    want = {"EAX", "EBX", "ECX", "EDX", "ESI", "EDI", "EBP", "ESP", "EIP"}
    out = {}
    for i, rv in enumerate(vals):
        try:
            ri = dbg.regs(i)
            nm = ri.name.upper()
            if nm not in want or nm in out:
                continue
            v = rv.pyval(ri.dtype)
            out[nm] = hex(v) if isinstance(v, int) else str(v)
        except Exception:
            continue
    return out


def _reg(regs, name):
    v = regs.get(name)
    return int(v, 16) if v else 0


def find_func_by_addr(ea):
    """ea 所属易语言函数 (精确到函数范围), 不在易语言函数内返回 None"""
    f = ida_funcs.get_func(ea)
    if f is None:
        return None
    info = _core().STATE["functions"].get(f.start_ea)
    if info and info.get("name"):
        return {"start": hex(f.start_ea), "name": info["name"],
                "type": info.get("type", ""), "control": info.get("control", "")}
    return None


def e_call_stack():
    """调用栈, 易语言帧给出中文函数名"""
    out = []
    try:
        tid = ida_dbg.get_current_thread()
        trace = ida_idd.call_stack_t()
        if not ida_dbg.collect_stack_trace(tid, trace):
            return []
        for frame in trace:
            ea = frame.callea
            ef = find_func_by_addr(ea)
            if ef:
                out.append({"address": hex(ea), "eFunction": ef["name"],
                            "type": ef["type"]})
            else:
                sym = ""
                try:
                    sym = ida_name.get_name(ea) or ""
                except Exception:
                    pass
                out.append({"address": hex(ea), "symbol": sym})
    except Exception:
        return []
    return out


def _lib_cmd_name(lib_func_ebx):
    """ebx 编码 (库索引<<16|命令索引, 从1计) -> 支持库命令名"""
    try:
        lib_index = (lib_func_ebx >> 0x10) - 1
        cmd_index = (lib_func_ebx & 0xFFFF) - 1
        libs = _core().STATE["libs"]
        if lib_index < 0 or lib_index >= len(libs):
            return None
        cmds = libs[lib_index]["cmds"]
        if cmd_index < 0 or cmd_index >= len(cmds):
            return None
        cmd = cmds[cmd_index]
        if isinstance(cmd, dict):
            return "%s.%s" % (libs[lib_index]["name"], cmd.get("name") or ("命令#%d" % cmd_index))
        return "%s.命令#%d(未识别)@%s" % (libs[lib_index]["name"], cmd_index, hex(cmd))
    except Exception:
        return None


def _import_candidates(index):
    """DLL命令索引 -> 候选 API (索引基序不确定, 同时给出 idx 与 idx-1)"""
    imports = _core().STATE["imports"]
    cands = []
    for label, i in (("idx-1", index - 1), ("idx", index)):
        if 0 <= i < len(imports):
            cands.append({"which": label, "index": i,
                          "full": imports[i].get("full") or imports[i].get("api")})
    return cands


def _control_and_property(window_id, control_id, prop_index):
    """(windowId, controlId, 属性索引) -> 控件名/属性名"""
    ctrl = _core().CONTROL_INDEX.get((window_id, control_id))
    if not ctrl:
        return None
    try:
        prop = _core()._property_name(ctrl, prop_index)
    except Exception:
        prop = "属性%d" % prop_index
    return {"control": ctrl.get("name"), "type": ctrl.get("controlTypeName"),
            "property": prop}


def _read_stack_args(esp, count, skip):
    """从 esp+skip 开始读 count 个 dword"""
    vals = []
    for i in range(count):
        v = read_mem_u32(esp + skip + 4 * i)
        if v is None:
            break
        vals.append(v)
    return vals


def interpret_krnl_hit(func_type, regs):
    """断在 krnln 跳转入口时的现场解释"""
    esp, eax, ebx = _reg(regs, "ESP"), _reg(regs, "EAX"), _reg(regs, "EBX")
    out = {}
    if func_type == "Jmp_MCallDllCmd":
        out["meaning"] = "正在调用 DLL命令 (参数: eax=导入表索引)"
        out["apiIndex"] = hex(eax)
        out["apiCandidates"] = _import_candidates(eax)
    elif func_type == "Jmp_MCallLibCmd":
        out["meaning"] = "正在调用 三方支持库命令 (参数: ebx=库命令索引)"
        out["cmdIndex"] = hex(ebx)
        out["cmdName"] = _lib_cmd_name(ebx)
    elif func_type == "Jmp_MCallKrnlLibCmd":
        out["meaning"] = "正在调用 核心支持库命令 (参数: ebx=库命令索引)"
        out["cmdIndex"] = hex(ebx)
        out["cmdName"] = _lib_cmd_name(ebx)
    elif func_type == "Jmp_MMalloc":
        out["meaning"] = "正在分配内存 (参数: [esp+4]=字节数)"
        vals = _read_stack_args(esp, 1, 4)
        out["bytes"] = hex(vals[0]) if vals else None
    elif func_type == "Jmp_MFree":
        out["meaning"] = "正在释放内存 (参数: [esp+4]=地址)"
        vals = _read_stack_args(esp, 1, 4)
        out["ptr"] = hex(vals[0]) if vals else None
    elif func_type in ("Jmp_MWriteProperty", "Jmp_MReadProperty"):
        verb = "写" if func_type == "Jmp_MWriteProperty" else "读"
        out["meaning"] = "正在%s组件属性 (栈参数: 窗口ID,控件ID,属性索引,...)" % verb
        args = _read_stack_args(esp, 3, 4)
        if len(args) == 3:
            hit = _control_and_property(args[0], args[1], args[2])
            if hit:
                out["target"] = hit
                out["rawArgs"] = [hex(v) for v in args]
            else:
                out["rawArgs"] = [hex(v) for v in args]
                out["note"] = "窗口/控件ID未匹配到已解析控件 (读属性场景可能需要运行时值)"
    elif func_type == "Jmp_MReportError":
        out["meaning"] = "错误回调 (易语言运行时错误报告), 可查看栈参数定位出错调用"
    elif func_type == "Jmp_MMessageLoop":
        out["meaning"] = "窗口消息循环"
    elif func_type == "Jmp_MLoadBeginWin":
        out["meaning"] = "载入启动窗口"
    elif func_type == "Jmp_MExitProcess":
        out["meaning"] = "程序结束"
    return out or None


def context():
    """当前暂停现场: 所在E函数/寄存器/中文调用栈/krnln现场解释/断点列表"""
    ip = require_suspended()
    state = _core().STATE
    regs = get_key_registers()
    out = {"ip": hex(ip), "registers": regs}

    cur = find_func_by_addr(ip)
    out["currentFunction"] = cur
    if cur and str(cur.get("type", "")).startswith("Jmp_"):
        out["krnlHit"] = interpret_krnl_hit(cur["type"], regs)
    out["callStack"] = e_call_stack()
    out["breakpoints"] = list_breakpoints()
    return out


# ---------------------------------------------------------------------------
# 数据解释: 栈参数 / 易语言文本
# ---------------------------------------------------------------------------


def _describe_value(v, text_bytes=64):
    """猜测一个 dword 值的易语言语义"""
    info = _core().STATE["functions"].get(v)
    if info and info.get("name"):
        return "函数地址 %s" % info["name"]
    if ida_segment.getseg(v) is not None:
        raw = read_mem(v, text_bytes)
        if raw:
            end = raw.find(b"\x00")
            chunk = raw[:end] if end != -1 else raw
            if chunk:
                try:
                    text = chunk.decode("gbk")
                except UnicodeDecodeError:
                    text = None
                if text and all(c.isprintable() for c in text):
                    return "文本指针? '%s'" % text
        return "映像内地址"
    return ""


def dump_args(count=8):
    """dump 当前易语言函数的栈参数并逐个解释"""
    ip = require_suspended()
    if count < 1 or count > 64:
        raise RuntimeError("count 取值范围 1-64")
    regs = get_key_registers()
    esp, ebp = _reg(regs, "ESP"), _reg(regs, "EBP")

    base, via = None, None
    cur = find_func_by_addr(ip)
    if cur:
        start = _parse_hex(cur["start"])
        # 断点落在函数入口(序言未执行)时用 esp+4; 序言(push ebp; mov ebp,esp)
        # 已执行且 ebp 链看似有效时用 ebp+8
        if ip >= start + 3 and _valid_ebp_frame(ebp, esp):
            base, via = ebp + 8, "ebp+8 (函数序言已执行)"
    if base is None:
        base, via = esp + 4, "esp+4 (断点在入口附近, 按调用瞬间栈布局)"

    vals = _read_stack_args(base, count, 0)
    args = []
    for i, v in enumerate(vals):
        args.append({"index": i, "offset": hex(base + 4 * i), "value": hex(v),
                     "meaning": _describe_value(v)})
    return {"function": cur, "argsBase": via, "args": args,
            "note": "易语言静态编译多为 cdecl/stdcall 混用, 此处只做栈面解释"}


def _valid_ebp_frame(ebp, esp):
    """校验 ebp 是否像已建立的栈帧: [ebp] 为更高地址的上级帧, [ebp+4] 为映像内返回地址"""
    if not ebp or ebp <= esp or ebp - esp > 0x100000:
        return False
    saved = read_mem_u32(ebp)
    ret = read_mem_u32(ebp + 4)
    if saved is None or ret is None:
        return False
    if saved <= ebp or saved - ebp > 0x100000:
        return False
    return ida_segment.getseg(ret) is not None


def read_text(ea, max_len=256):
    """按易语言文本型语义读取字符串 (GBK, 指针指向数据区, 前4字节常为长度)"""
    ea = _parse_hex(ea)
    if max_len < 1 or max_len > 4096:
        raise RuntimeError("max_len 取值范围 1-4096")
    raw = read_mem(ea, max_len)
    if raw is None:
        raise RuntimeError("无法读取地址 %s 处的内存 (目标未暂停或地址无效)" % hex(ea))
    end = raw.find(b"\x00")
    data = raw[:end] if end != -1 else raw
    out = {
        "address": hex(ea),
        "text": data.decode("gbk", errors="replace"),
        "bytesRead": len(data),
        "nullTerminated": end != -1,
    }
    prefix = read_mem_u32(ea - 4)
    if prefix is not None and prefix < 0x1000000:
        out["lengthPrefixHint"] = prefix
        if end != -1 and prefix != end:
            out["note"] = "长度前缀(%d)与\\0位置(%d)不一致, 可能不是易语言动态串" % (prefix, end)
    return out


# ---------------------------------------------------------------------------
# 运行控制辅助
# ---------------------------------------------------------------------------

def run_to_target(name):
    """让调试器运行到指定易语言目标 (需已启动调试); 多个匹配时拒绝执行"""
    hits = resolve_targets(name)
    if not hits:
        raise RuntimeError("未找到匹配的易语言目标: %s" % name)
    if len(hits) > 1:
        return {"error": "匹配到 %d 个目标, 请给出更精确的名称" % len(hits),
                "matches": hits[:8]}
    ea = _parse_hex(hits[0]["address"])
    if not ida_dbg.run_to(ea):
        raise RuntimeError("run_to 失败, 调试器可能未启动")
    return {"ranTo": hits[0]}
