# e-mcp — 易语言程序分析 MCP 工具 (纯 Python / IDAPython)

把本仓库 E-Decompiler 插件的核心解析逻辑移植为纯 Python (IDAPython),
运行在 **IDA 9.x** 上, 并通过 [ida-pro-mcp](https://github.com/mrexodia/ida-pro-mcp)
暴露为 [MCP](https://modelcontextprotocol.io) 工具, 供 AI 客户端 (Claude/ZCode 等)
直接调用。**不需要 IDA SDK, 不需要编译 C++。**

## 已移植的功能

| 原项目 (C++) | 本项目 (Python) | 说明 |
|---|---|---|
| SectionManager | `Memory` | 全部段读入缓冲区, 偏移/线性地址互转 |
| 静态特征探测 | `analyze()` 第1步 | `50 64 89 25...` 特征 + 易语言头校验 |
| makeFunction | `analyze()` 第2步 | 扫描 `55 8B EC` 批量创建函数 |
| ESymbol::loadELibInfomation | 第3步 | 支持库/数据类型/命令表解析 |
| TrieTree 特征码引擎 | `TrieTree` 类 | esig 文本格式, 完整移植(含半字节通配/子函数/CALL API/长跳转) |
| scanELibFunction | 第4步 | 用 `plugins\esig` 识别核心支持库函数并命名 |
| loadKrnlInterface | 第6步 | krnln 跳转表定位(DLL命令/错误回调/读写属性等13个), 并应用原型 |
| scanEClassTable | 第7步 | 类虚表扫描(命名 vtable_XXXXXXXX) |
| loadGUIResource | 第8步 | 窗口/控件/菜单解析 + 事件处理函数命名(`_按钮1_被单击`) |
| loadUserImports | 第9步 | DLL 命令(用户导入表) |
| 29 个控件事件表 | `event_tables.json` | 从原项目 EAppControl/*.cpp 自动提取 |

未移植: 原 C++ 插件的 Qt 界面(属性窗口等)与特征码制作器(ECSigMaker)。
这两部分只影响 IDA 交互体验, 不影响分析能力。

## Hex-Rays 反编译修正

原项目 CTreeFixer/MicroCodeFixer 已移植为 `e_mcp_hexrays.py`, 利用
`ida_hexrays` 的 Python 绑定(IDA 9.x 自带), `elang_analyze()` 成功后自动安装钩子:

- **读取组件属性** 调用 → 改写为 `类型::控件名_读属性_属性名`(如 `编辑框::编辑框1_读属性_内容`)
- **设置组件属性** 调用 → 改写为 `类型::控件名_写属性_属性名`
- **DLL 命令调用** → 改写为真实 API 名(依赖导入表解析顺序)
- **错误回调模式**(microcode 层): `条件跳转 + call 错误回调` 改写为 goto,
  消除反编译输出中的错误处理噪音

27 个控件的属性名表同样从 C++ 源码自动提取(见 `event_tables.json` 的 `properties`)。

## 易语言动态调试

`e_mcp_dbg.py` 在 ida-pro-mcp 通用调试器工具(`dbg_start_process` /
`dbg_continue_process` / `dbg_step_into` / `dbg_step_over` /
`dbg_get_registers` / `decompile_function` ...)之上叠加易语言语义:

- **按中文名下断点**: 事件处理函数(`_保存_被单击`)、krnln 语义入口
  (DLL命令/错误回调/读写属性/分配内存...)、特征码识别出的支持库函数
- **暂停现场解读** `elang_dbg_context()`: 当前 E 函数、关键寄存器、
  中文调用栈、krnln 调用现场解释
- **krnln 现场解释**: 断在 DLL命令 → 报告正在调用哪个 API(eax=导入表索引);
  断在支持库命令 → 报告命令名(ebx=库命令索引); 断在写属性 → 报告
  目标控件与属性名(栈参数); 断在分配内存 → 报告字节数
- **易语言数据解释**: `elang_dbg_args()` dump 栈参数并逐个解释值语义;
  `elang_dbg_read_text()` 按文本型语义读 GBK 字符串(含长度前缀提示)

典型调试流程:

1. `elang_dbg_targets()` — 浏览可断点目标(事件函数/krnln入口)
2. `elang_dbg_break("被单击")` — 按名称子串下断点(可命中多个)
3. `dbg_start_process()` — 启动调试(通用工具)
4. 断点命中后 `elang_dbg_context()` — 解读当前现场
5. `elang_dbg_args()` / `elang_dbg_read_text()` — 看参数与字符串
6. `dbg_step_into` / `decompile_function` / `dbg_continue_process` ...

## MCP 工具(16 个)

易语言静态分析(9 个):

- `elang_analyze()` — 一键解析当前 IDB
- `elang_status()` — 解析状态与概览
- `elang_list_functions(offset, count)` — 分页列出识别出的函数
- `elang_find_function(name)` — 按中文名搜函数(支持子串)
- `elang_list_controls(offset, count)` — 窗口/控件/菜单及事件
- `elang_get_control(window_id, control_id)` — 控件详情
- `elang_list_libs()` — 支持库与命令(含识别结果)
- `elang_list_imports()` — DLL 命令
- `elang_list_vtables()` — 类虚表

易语言动态调试(6 个, 需先 `elang_analyze()`):

- `elang_dbg_targets(filter)` — 列出可断点目标(事件函数/krnln入口)
- `elang_dbg_break(target)` — 按中文名子串/krnln语义名设置断点
- `elang_dbg_context()` — 解读当前暂停现场(E函数/寄存器/中文调用栈/krnln解释)
- `elang_dbg_args(count)` — dump 当前函数栈参数并解释值语义
- `elang_dbg_read_text(ea, max_len)` — 按文本型语义读 GBK 字符串
- `elang_dbg_run_to(target)` — 运行到指定易语言目标

通用工具(1 个, 由安装器追加):

- `exec_idapython(code)` — 在 IDA 主线程执行任意 IDAPython 代码

## 安装

前置: IDA 9.x(需 9.0+, 实测 9.4), IDA 所用的 Python 3.11+, 且已安装
ida-pro-mcp (`pip install ida-pro-mcp`)。

```
<IDA所用的python> install.py [--ida-dir "D:\IDA Professional 9.4"]
```

安装内容:
- `e_mcp_*.py`、`event_tables.json`、`esig\` → `<IDA>\plugins\`
- `elang_*` 工具追加到 site-packages 的 `ida_pro_mcp\mcp-plugin.py`
  (server.py 启动时自动枚举 `@jsonrpc` 函数为 MCP 工具, 无需改 server)
- (默认)给 mcp-plugin.py 打上 server 随 IDA 自动启动的小补丁,
  `--no-autostart` 可关闭, 关闭后用 `Ctrl-Alt-M` 手动启动 server
- 同步打好补丁的 mcp-plugin.py 到 IDA plugins 目录(IDA 侧运行)

脚本可重复运行(幂等); 卸载时删除 IDA plugins 里的 e_mcp_* 文件并重装
ida-pro-mcp 即可。

## 使用流程

1. 启动 IDA 9.x, 打开易语言程序的 IDB(需先让 IDA 完成自动分析)
2. MCP server 随 IDA 自动启动(或 `Ctrl-Alt-M` 手动启动, 端口 13337)
   - 也可用 `Edit → Plugins → E-MCP: 解析易语言程序` 直接在 IDA 里跑分析
3. 在 MCP 客户端里即可调用 `elang_*` 工具

## 项目文件

- `e_mcp_core.py` — 解析引擎(IDAPython, IDA 9.x 兼容)
- `e_mcp_hexrays.py` — Hex-Rays 反编译修正(ctree/microcode, 纯 Python)
- `e_mcp_dbg.py` — 易语言动态调试语义层(断点/现场解读/数据解释)
- `e_mcp_extension.py` — MCP 工具定义(安装时追加到 mcp-plugin.py)
- `e_mcp_plugin.py` — IDA 菜单插件入口
- `event_tables.json` — 控件事件名表 + 属性名表(自动提取)
- `test_e_mcp_dbg.py` — 调试语义层 mock 测试(`python test_e_mcp_dbg.py`)
- `esig/` — 特征库(来自本项目发布包, 文本格式, 可直接扩充)
- `install.py` — 一键安装(重复运行安全, 幂等)
