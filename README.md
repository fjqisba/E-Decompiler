# E-Decompiler
用来辅助分析易语言程序的IDA 7.5插件，实验性项目。

### 如何编译项目

开发环境为: Windows + Visual Studio 2019 + Qt 5.6.3.0 + IDA SDK75

- 因为IDA自身使用的QT版本为Qt 5.6.3.0，因此必须和它保持同步，目前有效的下载地址为:https://download.qt.io/new_archive/qt/5.6/5.6.3/qt-opensource-windows-x86-msvc2015_64-5.6.3.exe
- 使用了Qt Vs插件，目前有效的下载地址为:https://download.qt.io/official_releases/vsaddin/
- 采用IDA SDK75版本的头文件和lib库，如果你缺少dirtree.hpp这个头文件，那么可以从IDA SDK76中拿过来使用。

配置选项:

- `C/C++` -> `附加包含目录` 需要添加IDA SDK的头文件目录，例如C:\MyLib\idasdk75\include
- `C/C++` -> `预处理器定义` 需要添加 `QT_NAMESPACE=QT`这一行
- `C/C++` -> `代码生成` -> `安全检查` 设置为`禁用安全检查(/GS-)`
- `链接器` -> `附加库目录` 需要添加IDA的Vc Lib文件目录和Qt Lib目录，例如C:\MyLib\idasdk75\lib\x64_win_vc_32和C:\MyLib\idasdk75\lib\x64_win_qt
- `链接器` -> `附加依赖项` 添加需要的lib文件，有ida.lib、Qt5Core.lib、Qt5Gui.lib、Qt5Widgets.lib、user32.lib...

欢迎给该项目提交Pull Request。

#### 开发者日志

2021年2月23日

完成对易语言常量资源的解析

2021年3月6日

继续完善对易语言常量资源的解析

2021年3月7日

还需要对易语言常量资源中的虚表进行解析...

2021年3月19日

重写易语言常量资源解析，测试版发布

2021年3月21日

完成控件资源的初步解析。

2021年3月23日

暂时先整理输出控件资源的解析代码，后面有空再进行优化。

2021年3月30日

引入QT界面框架。

2021年3月31日

完善常量资源解析中遗漏的跳转表解析。

控件资源界面设计已完成。

2021年4月1日

还需要写一个PE解析器

2021年4月2日

开始对控件开始添加属性

2021年4月5日

反序列化窗口属性，代码目前比较乱，需要整合。

2021年4月6日

对窗口属性进行人工解析。

2021年4月7日

开始解析标签控件