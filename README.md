# E-Decompiler

用来辅助分析易语言程序的IDA插件，实验性项目。支持 IDA 7.5 / IDA 9.x (推荐 9.4)。

反编译思路一:

基于retdec项目，将代码转换成LLVM IR后，结合易语言函数特征编写LLVM PASS来修正LLVM IR。

反编译思路二:

基于Ghidra项目，将汇编代码转换成PCode后，结合易语言函数特征编写Action来修正PCode。

反编译思路三:

基于IDA SDK，通过修改函数声明和CTree结构，结合易语言函数特征来优化反编译代码。



目前在尝试的是思路三。

### 如何编译项目

**IDA 9.x (推荐)**: 开发环境为 Windows + Visual Studio 2022 (MSVC v143) + IDA SDK 9.4，
使用 SDK 自带的 CMake 构建，无需安装 Qt (Qt 属性窗口组件暂未迁入 9.4 构建):

```powershell
cd E-Decompiler
cmake -S . -B build -DIDASDK="<IDA SDK 根目录>"   # SDK: github.com/HexRaysSA/ida-sdk
cmake --build build --config Release
```

产物为 `E-Decompiler.dll`，复制到 `<IDA>\plugins\` 即可。
迁移细节 (API 适配 / HEXRAYS_API_MAGIC 版本匹配 / UTF-8 编码策略) 见 [IDA9.4-PORT.md](IDA9.4-PORT.md)。

**IDA 7.5 (历史)**: 开发环境为: Windows + Visual Studio 2019 + Qt 5.6.3.0 + IDA SDK75

- 因为IDA自身使用的QT版本为Qt 5.6.3.0，因此必须和它保持同步，目前有效的下载地址为:https://download.qt.io/new_archive/qt/5.6/5.6.3/qt-opensource-windows-x86-msvc2015_64-5.6.3.exe
- 使用了Qt Vs插件，目前有效的下载地址为:https://download.qt.io/official_releases/vsaddin/

配置选项:

- `C/C++` -> `附加包含目录` 需要添加IDA SDK的头文件目录，例如C:\MyLib\idasdk75\include
- `C/C++` -> `预处理器定义` 需要添加 `QT_NAMESPACE=QT`这一行
- `C/C++` -> `代码生成` -> `安全检查` 设置为`禁用安全检查(/GS-)`
- `链接器` -> `附加库目录` 需要添加IDA的Vc Lib文件目录和Qt Lib目录，例如C:\MyLib\idasdk75\lib\x64_win_vc_32和C:\MyLib\idasdk75\lib\x64_win_qt
- `链接器` -> `附加依赖项` 添加需要的lib文件，有ida.lib、Qt5Core.lib、Qt5Gui.lib、Qt5Widgets.lib、user32.lib...

### 使用说明

1. (仅IDA 7.5) 需要先给IDA做一个patch，使之支持中文函数。详细情况见[IDA7.5支持中文函数命名的办法](https://www.52pojie.cn/thread-1414525-1-1.html)；IDA 9.x 原生支持中文函数名，无需此步骤。
2. 将E-Decompiler.dll和esig文件夹放置于插件目录，例如D:\IDA Pro 9.4\plugins
3. 运行IDA后，按Ctrl+3快捷键呼出插件菜单，运行插件即可。

### E-MCP: 纯 Python 版 + AI Agent (MCP) 支持

[e-mcp/](e-mcp/README.md) 将本插件的核心解析逻辑 (含Hex-Rays反编译修正) 移植为
纯 IDAPython，**无需 IDA SDK、无需编译**，即可在 IDA 9.x 上运行；并通过
[ida-pro-mcp](https://github.com/mrexodia/ida-pro-mcp) 暴露为 MCP 工具，
让 Claude / ZCode 等 AI 客户端可以直接解析易语言程序、按中文事件名下断点、
解读调试现场。一键安装: `python e-mcp/install.py`。

### 项目进度

- [x] 使IDA支持中文函数命名。
- [x] 易语言控件的彻底解析。
- [x] 易语言库函数的特征码识别引擎移植。
- [ ] 易语言模块函数的特征码提取与识别。

反编译核心部分

- [x] 控件的交叉引用，控件事件的展示。
- [ ] 易语言基础特征函数的优化、易语言数组操作。
- [x] DLL命令调用函数的优化。
- [x] 错误回调函数的优化。

本项目完全是由个人利用业余时间开发，不定时更新。



### 关于模块函数特征识别

通过定制化开发一套轻量级的特征识别引擎，来实现模块函数特征识别。 



### 参考资料

https://chaotic.gd/ghidra-decompiler-docs-built/index.html

[Online Courses – Ghidra](https://ghidra.re/online-courses/)

[NationalSecurityAgency/ghidra: Ghidra is a software reverse engineering (SRE) framework](https://github.com/NationalSecurityAgency/ghidra)

[airbus-cert/Yagi: Yet Another Ghidra Integration for IDA](https://github.com/airbus-cert/Yagi)

[avast/retdec: RetDec is a retargetable machine-code decompiler based on LLVM. (github.com)](https://github.com/avast/retdec)

[RolfRolles/HexRaysDeob: Hex-Rays microcode API plugin for breaking an obfuscating compiler (github.com)](https://github.com/RolfRolles/HexRaysDeob)

[patois/HRDevHelper: Context-sensitive HexRays decompiler plugin that visualizes the ctree of decompiled functions. (github.com)](https://github.com/patois/HRDevHelper)

https://github.com/OpenEpl/TextECode

NjMzNzgzNjAy
