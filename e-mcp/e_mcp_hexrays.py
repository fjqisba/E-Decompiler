# -*- coding: utf-8 -*-
"""
e_mcp_hexrays.py — Hex-Rays 反编译输出修正 (纯 Python, 无需 IDA SDK)

移植自 fjqisba/E-Decompiler 的 CTreeFixer.cpp:
 1. CTree 修正 (hxe_maturity == CMAT_FINAL):
    - 读取组件属性调用  -> 改写为 `类型::控件名_读属性_属性名` 辅助函数
    - 设置组件属性调用  -> 改写为 `类型::控件名_写属性_属性名` 辅助函数
    - DLL 命令调用      -> 改写为实际的 API 名 (kernel32.CreateFileA 等)
 2. Microcode 修正 (hxe_microcode):
    - 错误回调模式: `条件跳转 + call 错误回调` 的块, 把 jcnd 转成 goto,
      消除反编译输出里大量的错误处理噪音

依赖 e_mcp_core 的解析结果 (STATE), 在 analyze() 成功后自动安装钩子。
"""

import ida_hexrays

import e_mcp_core as core


class _CTreeFixerVisitor(ida_hexrays.ctree_visitor_t):
    def __init__(self, state):
        super().__init__(ida_hexrays.CV_FAST)
        self.state = state

    def _num(self, carg):
        """取调用参数的立即数值, 不是数字返回 None"""
        try:
            n = carg.n
            if n is None:
                return None
            if n.op != ida_hexrays.cot_num:
                return None
            return n.numval() & 0xFFFFFFFF
        except Exception:
            return None

    def _make_helper(self, e, name):
        helper = ida_hexrays.create_helper(True, e.a.functype, "%s", name)
        e.x.cleanup()
        e.x.replace_by(helper)

    def _lookup_control(self, window_id, control_id):
        return core.CONTROL_INDEX.get((window_id, control_id))

    def _fix_read_property(self, e):
        if e.a.size() != 4:
            return
        wid = self._num(e.a[0])
        cid = self._num(e.a[1])
        pidx = self._num(e.a[2])
        ctrl = self._lookup_control(wid, cid) if None not in (wid, cid, pidx) else None
        if not ctrl:
            return
        name = "%s::%s_读属性_%s" % (ctrl["controlTypeName"], ctrl["name"],
                                     core._property_name(ctrl, pidx))
        self._make_helper(e, name)

    def _fix_write_property(self, e):
        if e.a.size() != 6:
            return
        wid = self._num(e.a[0])
        cid = self._num(e.a[1])
        pidx = self._num(e.a[2])
        ctrl = self._lookup_control(wid, cid) if None not in (wid, cid, pidx) else None
        if not ctrl:
            return
        name = "%s::%s_写属性_%s" % (ctrl["controlTypeName"], ctrl["name"],
                                     core._property_name(ctrl, pidx))
        self._make_helper(e, name)

    def _fix_dll_cmd(self, e):
        if e.a.size() < 1:
            return
        order = self._num(e.a[0])
        imports = self.state["imports"]
        if order is None or order >= len(imports):
            return
        self._make_helper(e, imports[order]["full"])

    def visit_expr(self, e):
        if e.op != ida_hexrays.cot_call:
            return 0
        x = e.x
        if x is None or x.op != ida_hexrays.cot_obj:
            return 0
        info = self.state["functions"].get(x.obj_ea)
        if not info:
            return 0
        ftype = info.get("type")
        try:
            if ftype == "Jmp_MReadProperty":
                self._fix_read_property(e)
            elif ftype == "Jmp_MWriteProperty":
                self._fix_write_property(e)
            elif ftype == "Jmp_MCallDllCmd":
                self._fix_dll_cmd(e)
        except Exception:
            pass
        return 0


def _fix_report_error_blocks(mba, state):
    """错误回调模式修复 (对应原项目 fixMicrocode_KrnlReportError):
    若块尾是 call 错误回调, 且前一块尾是条件跳转, 则把条件跳转改为无条件 goto"""
    report_addr = _krnl_addr(state, "Jmp_MReportError")
    if report_addr is None:
        return
    blk = mba.get_mblock(0)
    while blk is not None:
        tail = blk.tail
        if tail is not None and tail.opcode == ida_hexrays.m_call and tail.l.r == report_addr:
            prevb = blk.prevb
            if prevb is not None and prevb.tail is not None \
                    and prevb.tail.opcode == ida_hexrays.m_jcnd:
                ins = prevb.tail
                ins.opcode = ida_hexrays.m_goto
                ins.l.make_gvar(ins.d.g)
                ins.d.erase()
        blk = blk.nextb


def _krnl_addr(state, field):
    v = state.get("krnl", {}).get(field)
    return int(v, 16) if v else None


class HexRaysFixer:
    """管理 hexrays 回调的安装/卸载, 单例保存在模块变量 _fixer 中"""

    def __init__(self, state):
        self.state = state
        self._installed = False

    def _callback(self, event, *args):
        try:
            if event == ida_hexrays.hxe_maturity:
                cfunc, maturity = args[0], args[1]
                if maturity == ida_hexrays.CMAT_FINAL:
                    visitor = _CTreeFixerVisitor(self.state)
                    visitor.apply_to(cfunc.body, None)
            elif event == ida_hexrays.hxe_microcode:
                mba = args[0]
                _fix_report_error_blocks(mba, self.state)
        except Exception:
            pass
        return 0

    def install(self):
        if self._installed or not ida_hexrays.init_hexrays_plugin():
            return False
        ida_hexrays.install_hexrays_callback(self._callback)
        self._installed = True
        return True

    def uninstall(self):
        if self._installed:
            ida_hexrays.remove_hexrays_callback(self._callback)
            self._installed = False


_fixer = None


def install_fixer(state):
    """analyze() 成功后调用; 幂等"""
    global _fixer
    if _fixer is not None:
        _fixer.uninstall()
    _fixer = HexRaysFixer(state)
    ok = _fixer.install()
    import ida_kernwin
    ida_kernwin.msg("[E-MCP] Hex-Rays 反编译修正: %s\n" % ("已启用" if ok else "不可用(无Hex-Rays)"))
    return ok
