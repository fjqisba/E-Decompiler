# E-Decompiler
用来辅助分析易语言程序的IDA 7.5插件，实验性项目。

### 如何编译项目

开发环境为: Windows + Visual Studio 2019 + Qt 5.6.3.0 + IDA SDK75

- 因为IDA自身使用的QT版本为Qt 5.6.3.0，因此必须和它保持同步，目前有效的下载地址为:https://download.qt.io/new_archive/qt/5.6/5.6.3/qt-opensource-windows-x86-msvc2015_64-5.6.3.exe
- 使用了Qt Vs插件，目前有效的下载地址为:https://download.qt.io/official_releases/vsaddin/

配置选项:

- `C/C++` -> `附加包含目录` 需要添加IDA SDK的头文件目录，例如C:\MyLib\idasdk75\include
- `C/C++` -> `预处理器定义` 需要添加 `QT_NAMESPACE=QT`这一行
- `C/C++` -> `代码生成` -> `安全检查` 设置为`禁用安全检查(/GS-)`
- `链接器` -> `附加库目录` 需要添加IDA的Vc Lib文件目录和Qt Lib目录，例如C:\MyLib\idasdk75\lib\x64_win_vc_32和C:\MyLib\idasdk75\lib\x64_win_qt
- `链接器` -> `附加依赖项` 添加需要的lib文件，有ida.lib、Qt5Core.lib、Qt5Gui.lib、Qt5Widgets.lib、user32.lib...

欢迎给该项目提交Pull Request。

### 使用说明

1. 在使用本插件之前，需要给IDA7.5做一个patch，使之支持中文函数。详细情况见[IDA7.5支持中文函数命名的办法](https://www.52pojie.cn/thread-1414525-1-1.html)
2. 由于IDA自身使用的QT不支持图片显示，需要将imageformats目录(里面是QT图片格式插件)放置于IDA主程序目录下。
3. 将E-Decompiler.dll和esig文件夹放置于插件目录，例如D:\IDA 7.5 SP3\plugins
4. 运行IDA后，按Ctrl+3快捷键呼出插件菜单，运行插件即可。

### 项目进度

- [x] 基于交叉引用的常量资源解析引擎
- [x] 使IDA支持中文函数命名。
- [ ] 易语言控件的彻底解析(已搭建好解析框架)
- [x] 易语言库函数的特征码识别引擎移植。
- [ ] 易语言模块函数的特征码提取与识别。
- [ ] 反编译引擎框架。

本项目完全是由个人利用业余时间开发，不定时更新。

### 反编译步骤

1.去除易语言花指令(可暂时略过)

2.根据易语言汇编代码特性创建中间指令

3.易语言编译器代码和置入代码识别



