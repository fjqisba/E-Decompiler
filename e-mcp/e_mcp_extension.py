
# ======================================================================
# ==== e-mcp extension: 易语言(E-Language)分析工具 ====
# 本块由 e-mcp/install.py 追加到 ida-pro-mcp 的 mcp-plugin.py 末尾,
# 重装时会先移除旧块再追加, 请勿手工编辑
# ======================================================================

_E_MCP_LOAD_MARK = {"mtime": None}


def _e_mcp_import_core():
    import importlib
    import os
    import sys
    core = sys.modules.get("e_mcp_core")
    if core is None:
        core = importlib.import_module("e_mcp_core")
    # 仅当 e_mcp_core.py 文件发生变化时才重载, 保证 STATE 跨调用保持
    core_dir = os.path.dirname(os.path.abspath(core.__file__))
    mtime = os.path.getmtime(os.path.join(core_dir, "e_mcp_core.py"))
    if core_dir not in sys.path:
        sys.path.insert(0, core_dir)
    if _E_MCP_LOAD_MARK.get("mtime") != mtime:
        if _E_MCP_LOAD_MARK["mtime"] is not None:
            importlib.reload(core)
        try:
            hr = importlib.import_module("e_mcp_hexrays")
            if _E_MCP_LOAD_MARK["mtime"] is not None:
                importlib.reload(hr)
        except ImportError:
            pass
        _E_MCP_LOAD_MARK["mtime"] = mtime
    return core


def _e_mcp_sync(fn):
    """把函数调度到 IDA 主线程执行"""
    import idaapi
    box = {}

    def runner():
        try:
            box["result"] = fn()
        except Exception as e:  # noqa: BLE001
            box["error"] = e

    idaapi.execute_sync(runner, idaapi.MFF_FAST)
    if "error" in box:
        raise box["error"]
    return box.get("result")


def _e_mcp_json(obj):
    import json
    return json.dumps(obj, ensure_ascii=False, indent=1, default=str)


@jsonrpc
def elang_analyze() -> str:
    "解析当前 IDB 中的易语言静态编译程序: 创建函数、识别支持库函数(需要esig特征库)、命名事件处理函数、解析控件与导入表"
    def work():
        core = _e_mcp_import_core()
        state = core.analyze()
        if state["error"]:
            raise IDAError(state["error"])
        return state["info"]
    return _e_mcp_json(_e_mcp_sync(work))


@jsonrpc
def elang_status() -> str:
    "获取易语言解析状态与概览信息(是否已解析、代码段范围、函数/控件/事件数量等)"
    def work():
        core = _e_mcp_import_core()
        state = core.STATE
        return {
            "analyzed": state["analyzed"],
            "error": state["error"],
            "info": state["info"],
        }
    return _e_mcp_json(_e_mcp_sync(work))


@jsonrpc
def elang_list_functions(
    offset: Annotated[int, "起始索引"],
    count: Annotated[int, "返回数量"],
) -> str:
    "分页列出识别出的易语言相关函数(库函数/事件处理函数/krnln跳转等), 按地址排序"
    def work():
        core = _e_mcp_import_core()
        items = sorted(core.STATE["functions"].items())
        result = []
        for addr, info in items[offset:offset + count]:
            entry = dict(info)
            entry["address"] = hex(addr)
            result.append(entry)
        return {"total": len(items), "functions": result}
    return _e_mcp_json(_e_mcp_sync(work))


@jsonrpc
def elang_find_function(
    name: Annotated[str, "函数名(支持子串匹配, 如 按钮1 或 被单击)"],
) -> str:
    "按名称搜索易语言函数(支持中文), 返回地址与类型"
    def work():
        core = _e_mcp_import_core()
        result = []
        for addr, info in sorted(core.STATE["functions"].items()):
            if name in (info.get("name") or ""):
                entry = dict(info)
                entry["address"] = hex(addr)
                result.append(entry)
        return {"count": len(result), "functions": result}
    return _e_mcp_json(_e_mcp_sync(work))


@jsonrpc
def elang_list_controls(
    offset: Annotated[int, "起始索引"],
    count: Annotated[int, "返回数量"],
) -> str:
    "分页列出解析出的窗口/控件/菜单及其事件处理函数"
    def work():
        core = _e_mcp_import_core()
        ctrls = core.STATE["controls"]
        return {
            "total": len(ctrls),
            "controls": ctrls[offset:offset + count],
        }
    return _e_mcp_json(_e_mcp_sync(work))


@jsonrpc
def elang_get_control(
    window_id: Annotated[str, "窗口ID, 支持 0x 十六进制或十进制"],
    control_id: Annotated[str, "控件ID, 支持 0x 十六进制或十进制"],
) -> str:
    "按窗口ID+控件ID查询控件详情(属性、事件列表)"
    def work():
        core = _e_mcp_import_core()
        wid = int(window_id, 0)
        cid = int(control_id, 0)
        for ctrl in core.STATE["controls"]:
            if int(ctrl["windowId"], 0) == wid and int(ctrl["controlId"], 0) == cid:
                return ctrl
        raise IDAError("控件不存在: window=%s control=%s" % (window_id, control_id))
    return _e_mcp_json(_e_mcp_sync(work))


@jsonrpc
def elang_list_libs() -> str:
    "列出现解析出的易语言支持库、数据类型及各库命令函数(含特征码识别出的中文名)"
    def work():
        core = _e_mcp_import_core()
        return {
            "libs": core.STATE["libs"],
            "unmatchedCmds": core.STATE["unmatched_cmds"],
        }
    return _e_mcp_json(_e_mcp_sync(work))


@jsonrpc
def elang_list_imports() -> str:
    "列出易语言声明的DLL命令(用户导入表)"
    def work():
        core = _e_mcp_import_core()
        return {"total": len(core.STATE["imports"]), "imports": core.STATE["imports"]}
    return _e_mcp_json(_e_mcp_sync(work))


@jsonrpc
def elang_list_vtables() -> str:
    "列出扫描到的易语言类虚表及其成员函数"
    def work():
        core = _e_mcp_import_core()
        return {"total": len(core.STATE["vtables"]), "vtables": core.STATE["vtables"]}
    return _e_mcp_json(_e_mcp_sync(work))


# ======================================================================
# ==== e-mcp extension: 易语言动态调试工具 (e_mcp_dbg) ====
# ======================================================================

_DBG_LOAD_MARK = {"mtime": None}


def _e_mcp_import_dbg():
    import importlib
    import os
    import sys
    dbg = sys.modules.get("e_mcp_dbg")
    if dbg is None:
        dbg = importlib.import_module("e_mcp_dbg")
    dbg_dir = os.path.dirname(os.path.abspath(dbg.__file__))
    if dbg_dir not in sys.path:
        sys.path.insert(0, dbg_dir)
    # 仅当 e_mcp_dbg.py 文件发生变化时才重载
    mtime = os.path.getmtime(os.path.join(dbg_dir, "e_mcp_dbg.py"))
    if _DBG_LOAD_MARK.get("mtime") != mtime:
        if _DBG_LOAD_MARK["mtime"] is not None:
            importlib.reload(dbg)
        _DBG_LOAD_MARK["mtime"] = mtime
    return dbg


@jsonrpc
@unsafe
def elang_dbg_targets(
    filter: Annotated[str, "名称过滤子串(留空返回全部事件与krnln入口)"],
) -> str:
    "列出可断点的易语言目标: 事件处理函数(中文名)、krnln语义入口(DLL命令/错误回调/读写属性/分配内存等)"
    def work():
        dbg = _e_mcp_import_dbg()
        return dbg.get_targets(filter)
    return _e_mcp_json(_e_mcp_sync(work))


@jsonrpc
@unsafe
def elang_dbg_break(
    target: Annotated[str, "断点目标: 事件函数名子串(如 保存/被单击) 或 krnln语义名(如 DLL命令/错误回调/写属性)"],
) -> str:
    "按易语言语义设置断点(名称子串可命中多个), 返回命中目标与设置结果; 之后用 dbg_start_process 启动调试"
    def work():
        dbg = _e_mcp_import_dbg()
        hits = dbg.resolve_targets(target)
        if not hits:
            raise IDAError("未找到匹配的易语言目标: %s" % target)
        if len(hits) > 8:
            return {"error": "匹配过多(%d)个, 未设置断点, 请给出更精确的名称" % len(hits),
                    "matches": hits[:8]}
        done, skipped = dbg.add_breakpoints(hits)
        result = {"matched": hits, "set": done,
                  "breakpoints": dbg.list_breakpoints()}
        if skipped:
            result["skipped"] = skipped
        return result
    return _e_mcp_json(_e_mcp_sync(work))


@jsonrpc
@unsafe
def elang_dbg_context() -> str:
    "解读当前调试暂停位置: 所在易语言函数(中文名)、关键寄存器、中文调用栈、krnln调用现场解释(DLL命令API名/支持库命令名/控件属性)、断点列表"
    def work():
        dbg = _e_mcp_import_dbg()
        return dbg.context()
    return _e_mcp_json(_e_mcp_sync(work))


@jsonrpc
@unsafe
def elang_dbg_args(
    count: Annotated[int, "参数个数(1-64)"],
) -> str:
    "dump 当前易语言函数的栈参数, 并逐个解释值语义(函数地址/文本型指针GBK值/映像地址)"
    def work():
        dbg = _e_mcp_import_dbg()
        return dbg.dump_args(count)
    return _e_mcp_json(_e_mcp_sync(work))


@jsonrpc
@unsafe
def elang_dbg_read_text(
    ea: Annotated[str, "文本型指针地址, 支持 0x 十六进制"],
    max_len: Annotated[int, "最大读取长度(1-4096)"],
) -> str:
    "按易语言文本型语义读取调试目标内存中的GBK字符串(指针指向数据区, 前4字节通常为长度)"
    def work():
        dbg = _e_mcp_import_dbg()
        return dbg.read_text(ea, max_len)
    return _e_mcp_json(_e_mcp_sync(work))


@jsonrpc
@unsafe
def elang_dbg_run_to(
    target: Annotated[str, "运行目标, 用法同 elang_dbg_break"],
) -> str:
    "让已启动的调试器运行到指定易语言目标(事件函数/krnln入口), 等价 elang_dbg_break+dbg_run_to 的组合"
    def work():
        dbg = _e_mcp_import_dbg()
        return dbg.run_to_target(target)
    return _e_mcp_json(_e_mcp_sync(work))


# ==== end e-mcp extension ====
