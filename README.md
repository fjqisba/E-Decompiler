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

### 项目进度

目前实现的有:

- 基于交叉引用的常量资源解析引擎
- 易语言所有的控件属性的解析框架(还需要补充控件。。。)
- 易语言所有的控件事件的解析框架(还需要补充控件事件类型。。。)
- 使IDA支持中文函数命名。

计划实现的有:

- 易语言函数的特征码识别引擎移植。
- 反编译引擎框架。

