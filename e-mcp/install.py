# -*- coding: utf-8 -*-
"""
e-mcp 一键安装脚本 (在普通 Python 下运行, 不是在 IDA 里)

作用:
  1. 把 e_mcp_*.py / event_tables.json / esig 特征库拷贝到 IDA 9.x 的 plugins 目录
  2. 给已安装的 ida-pro-mcp 的 mcp-plugin.py (site-packages) 追加 elang_* MCP 工具,
     并可选打上 "server 随 IDA 自动启动" 补丁和 exec_idapython 通用工具
  3. 把打好补丁的 mcp-plugin.py 同步到 IDA plugins 目录 (IDA 侧插件)

用法 (务必用 IDA 配置的那个 Python 运行, 需 Python 3.11+ 且已 `pip install ida-pro-mcp`):

  python install.py [--ida-dir "D:\\IDA Professional 9.4"] [--no-autostart]

IDA 目录也可以用环境变量 E_MCP_IDA_DIR 指定; 都不给出时自动探测常见安装路径。
脚本可重复运行 (幂等): 重装时只替换 e-mcp 自己的代码块, 不碰 mcp-plugin.py 其他内容。
"""
import argparse
import glob
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
MCP_PLUGIN_PATH = None  # 运行时定位: site-packages 里 ida-pro-mcp 的 mcp-plugin.py

CORE_FILES = ["e_mcp_core.py", "e_mcp_hexrays.py", "e_mcp_dbg.py",
              "event_tables.json", "e_mcp_plugin.py"]

EXT_START = "# ==== e-mcp extension"
EXT_END = "# ==== end e-mcp extension ===="
EXEC_START = "# ==== e-mcp exec_idapython"
EXEC_END = "# ==== end e-mcp exec_idapython ===="

# exec_idapython: 通用 IDAPython 执行通道, 对分析易语言以外的任务也有用。
# 单独成块 (不在 extension 块内), 重装时同样先移除旧块再追加。
EXEC_IDAPYTHON = '''

# ======================================================================
# ==== e-mcp exec_idapython: 通用 IDAPython 执行通道 ====
# ======================================================================

@jsonrpc
def exec_idapython(
    code: Annotated[str, "IDAPython 代码(exec 语义, IDA 主线程执行); 用 result=... 设置返回值"],
) -> str:
    "在 IDA 主线程执行任意 IDAPython 代码, 返回 {'result': repr(result), 'stdout': 打印输出}; 未设置 result 时为 None"
    def work():
        import io
        import contextlib
        g = {"result": None}
        buf = io.StringIO()
        with contextlib.redirect_stdout(buf):
            try:
                exec(code, g)
            except Exception as e:
                raise IDAError("%s: %s\\nstdout:\\n%s" % (type(e).__name__, e, buf.getvalue()))
        return {"result": repr(g.get("result")), "stdout": buf.getvalue()}

    box = {}

    def runner():
        try:
            box["result"] = work()
        except Exception as e:  # noqa: BLE001
            box["error"] = e

    idaapi.execute_sync(runner, idaapi.MFF_FAST)
    if "error" in box:
        raise box["error"]
    # ensure_ascii 保持响应为纯 ASCII(中文以 \\uXXXX 转义), 规避传输层的编码问题
    return json.dumps(box.get("result"))


# ==== end e-mcp exec_idapython ====
'''

AUTOSTART_SNIPPET = '''        # e-mcp auto-start: IDA 启动即拉起 MCP server, agent 无需人工按键
        try:
            self.server.start()
            print("[MCP] Server auto-started (http://localhost:13337)")
        except Exception as e:
            print(f"[MCP] auto-start skipped: {e}")
'''


def remove_block(code, start, end):
    return re.sub(r"\n%s.*?%s\n" % (re.escape(start), re.escape(end)), "", code, flags=re.S)


def find_ida_dir(explicit):
    candidates = []
    if explicit:
        candidates.append(explicit)
    env = os.environ.get("E_MCP_IDA_DIR")
    if env:
        candidates.append(env)
    patterns = []
    for root in [os.environ.get("PROGRAMFILES", r"C:\Program Files"),
                 os.environ.get("LOCALAPPDATA", "") + os.sep + "Programs",
                 "C:\\", "D:\\", "E:\\"]:
        if root:
            patterns += [os.path.join(root, "IDA*"), os.path.join(root, "Hex-Rays", "IDA*")]
    for pat in patterns:
        candidates += glob.glob(pat)
    for c in candidates:
        if any(os.path.isfile(os.path.join(c, exe))
               for exe in ("ida.exe", "idat.exe", "ida64.exe", "idat64.exe")):
            return os.path.abspath(c)
    return None


def find_mcp_plugin():
    """用当前 Python 定位 site-packages 里 ida-pro-mcp 的 mcp-plugin.py"""
    probe = ("import ida_pro_mcp, os; "
             "print(os.path.join(os.path.dirname(ida_pro_mcp.__file__), 'mcp-plugin.py'))")
    try:
        out = subprocess.check_output([sys.executable, "-c", probe], text=True).strip()
        if os.path.isfile(out):
            return out
    except subprocess.CalledProcessError:
        pass
    return None


# 原版 ida-pro-mcp 的 mcp-plugin.py 以 PLUGIN_ENTRY 结尾;
# 其后的所有内容都是 e-mcp 追加的 (含旧版本安装留下的无标记块), 重装时整体重写
PLUGIN_ENTRY_RE = re.compile(r"^def PLUGIN_ENTRY\(\):\r?\n    return MCP\(\)\r?\n?", re.M)


def patch_mcp_plugin(path, with_autostart):
    with open(path, "r", encoding="utf-8") as f:
        code = f.read()

    m = PLUGIN_ENTRY_RE.search(code)
    if m:
        code = code[:m.end()]
    else:
        # 兜底: PLUGIN_ENTRY 形态有变化 (ida-pro-mcp 版本差异) 时只移除标记块
        print("[!] 未识别 PLUGIN_ENTRY 结尾, 按标记块增量更新")
        code = remove_block(code, EXEC_START, EXEC_END)
        code = remove_block(code, EXT_START, EXT_END)
    code = code.rstrip("\n") + "\n"

    # 1. server 自动启动补丁 (幂等)
    if with_autostart and "[MCP] Server auto-started" not in code:
        anchor = 'print(f"[MCP] Plugin loaded'
        idx = code.find(anchor)
        if idx >= 0:
            code = code[:idx] + AUTOSTART_SNIPPET + code[idx:]
        else:
            print("[!] 未找到自动启动锚点 (ida-pro-mcp 版本差异?), 跳过自动启动补丁;"
                  " 仍可用 Ctrl-Alt-M 手动启动 server")

    # 2. exec_idapython 通用工具
    code = code.rstrip() + "\n" + EXEC_IDAPYTHON

    # 3. e-mcp 扩展块
    with open(os.path.join(HERE, "e_mcp_extension.py"), "r", encoding="utf-8") as f:
        ext = f.read()
    code = code.rstrip() + "\n" + ext

    with open(path, "w", encoding="utf-8") as f:
        f.write(code)
    print("[+] patched ->", path)


def deploy_plugins_dir(d, with_core):
    os.makedirs(d, exist_ok=True)
    shutil.copy2(MCP_PLUGIN_PATH, os.path.join(d, "mcp-plugin.py"))
    if with_core:
        for n in CORE_FILES:
            shutil.copy2(os.path.join(HERE, n), os.path.join(d, n))
        # esig 必须与 e_mcp_core.py 同目录 (core 按自身路径找 .\esig)
        shutil.copytree(os.path.join(HERE, "esig"), os.path.join(d, "esig"),
                        dirs_exist_ok=True)
    print("[+] deployed ->", d)


def main():
    global MCP_PLUGIN_PATH
    ap = argparse.ArgumentParser(description="e-mcp 安装器")
    ap.add_argument("--ida-dir", help="IDA 安装目录 (含 plugins), 也可用环境变量 E_MCP_IDA_DIR")
    ap.add_argument("--no-autostart", action="store_true",
                    help="不给 mcp-plugin.py 打 server 自动启动补丁")
    args = ap.parse_args()

    ida_dir = find_ida_dir(args.ida_dir)
    if not ida_dir:
        sys.exit("未找到 IDA 目录, 请用 --ida-dir 或环境变量 E_MCP_IDA_DIR 指定 (需含 ida.exe)")
    print("[i] IDA 目录:", ida_dir)
    idaplugins = os.path.join(ida_dir, "plugins")

    mcp_plugin = find_mcp_plugin()
    if not mcp_plugin:
        sys.exit("当前 Python (%s) 未安装 ida-pro-mcp, 请先: %s -m pip install ida-pro-mcp"
                 % (sys.executable, os.path.basename(sys.executable)))
    MCP_PLUGIN_PATH = mcp_plugin
    print("[i] ida-pro-mcp:", mcp_plugin)

    # 1. 核心模块 + 特征库 -> IDA plugins
    for n in CORE_FILES:
        shutil.copy2(os.path.join(HERE, n), os.path.join(idaplugins, n))
    shutil.copytree(os.path.join(HERE, "esig"), os.path.join(idaplugins, "esig"),
                    dirs_exist_ok=True)
    print("[+] e_mcp modules + esig ->", idaplugins)

    # 2. 给 site-packages 的 mcp-plugin.py 打补丁
    patch_mcp_plugin(mcp_plugin, not args.no_autostart)

    # 3. 同步打好补丁的 mcp-plugin.py 到 IDA plugins (安装目录 + 用户目录)
    deploy_plugins_dir(idaplugins, True)
    user_plugins = os.path.join(os.environ.get("APPDATA", ""), "Hex-Rays", "IDA Pro", "plugins")
    if os.path.isdir(user_plugins):
        deploy_plugins_dir(user_plugins, False)

    print("\n完成! 后续步骤:")
    print("  1. 启动 IDA 9.x, 打开易语言程序 idb")
    if not args.no_autostart:
        print("  2. MCP server 随 IDA 自动启动 (也可 Ctrl-Alt-M 手动启动)")
    else:
        print("  2. Ctrl-Alt-M (Edit -> Plugins -> MCP) 启动 MCP server")
    print("  3. 重启 MCP 客户端, 即可看到 elang_* 系列工具")


if __name__ == "__main__":
    sys.exit(main())
