# -*- coding: utf-8 -*-
"""
E-MCP IDA 插件入口 — 提供 Edit -> Plugins 菜单, 直接在 IDA 里运行易语言分析。
(MCP 工具由 mcp-plugin.py 中的 elang_* 函数提供, 与本文件互不影响)
"""
import ida_idaapi
import ida_kernwin


class EActionHandler(ida_kernwin.action_handler_t):
    def activate(self, ctx):
        def work():
            import e_mcp_core
            state = e_mcp_core.analyze()
            msg = state["error"] or ("解析完成: %s" % state["info"])
            ida_kernwin.info("[E-MCP]\n%s" % msg)
        ida_kernwin.execute_sync(work, ida_kernwin.MFF_FAST)
        return 1

    def update(self, ctx):
        return ida_kernwin.AST_ENABLE_ALWAYS


def PLUGIN_ENTRY():
    class EPlugin(ida_idaapi.plugin_t):
        flags = ida_idaapi.PLUGIN_KEEP
        comment = "E-MCP: 易语言程序分析"
        help = "E-MCP"
        wanted_name = "E-MCP: Analyze E-Language"
        wanted_hotkey = ""

        def init(self):
            desc = ida_kernwin.action_desc_t(
                "e_mcp_analyze", "E-MCP: 解析易语言程序", EActionHandler(), None, None, 0)
            ida_kernwin.register_action(desc)
            ida_kernwin.attach_action_to_menu("Edit/Plugins/", "e_mcp_analyze",
                                              ida_kernwin.SETMENU_APP)
            print("[E-MCP] loaded — Edit -> Plugins -> E-MCP: 解析易语言程序")
            return ida_idaapi.PLUGIN_KEEP

        def run(self, arg):
            pass

        def term(self):
            pass

    return EPlugin()
