# E-Decompiler — IDA 9.x 插件构建
# 用法 (SDK: https://github.com/HexRaysSA/ida-sdk, git clone 即可):
#   cmake -S . -B build -DIDASDK="<IDA SDK 根目录>"
#   cmake --build build --config Release
cmake_minimum_required(VERSION 3.25)

set(IDASDK "" CACHE PATH "IDA SDK root (git clone: HexRaysSA/ida-sdk)")
if(NOT IDASDK OR NOT EXISTS "${IDASDK}/cmake/idasdk_standalone.cmake")
    message(FATAL_ERROR
        "未找到 IDA SDK: ${IDASDK}\n"
        "请指定 SDK 根目录: cmake -S . -B build -DIDASDK=\"<SDK根目录>\"\n"
        "SDK 获取: https://github.com/HexRaysSA/ida-sdk")
endif()
include("${IDASDK}/cmake/idasdk_standalone.cmake")
project(EDecompiler CXX)
set(CMAKE_CXX_STANDARD 17)
include("${IDASDK}/cmake/idasdk_init.cmake")
find_package(idasdk REQUIRED)

# hexrays API 版本须与装机引擎匹配: 9.4.0 正式版 = 5 (默认); 早期 9.4 build(如 260610) = 4
set(EDEC_HEXRAYS_API_VERSION "5" CACHE STRING "hexrays API version: 5 = 9.4.0 release, 4 = earlier 9.4 builds")

# 注: ControlInfoWidget(.cpp/.h/.ui)/PropertyDelegate 是 7.5 时代的 Qt 属性查看窗口,
# 与其余模块零耦合, 9.4 版暂不参与构建 (SDK 不带 Qt6 头文件, 需另装 Qt6 才能启用)。
ida_add_plugin(E-Decompiler
    SOURCES
        EDecompiler.cpp
        ESymbol.cpp
        ECSigParser.cpp
        ImportsParser.cpp
        SectionManager.cpp
        TrieTree.cpp
        EAppControl/EAppControl.cpp
        EAppControl/EAppControlFactory.cpp
        EAppControl/CKrnl_window.cpp
        EAppControl/CKrnl_AnimateBox.cpp
        EAppControl/CKrnl_Button.cpp
        EAppControl/CKrnl_CheckBox.cpp
        EAppControl/CKrnl_ChkListBox.cpp
        EAppControl/CKrnl_ComboBox.cpp
        EAppControl/CKrnl_DrawPanel.cpp
        EAppControl/CKrnl_DropTarget.cpp
        EAppControl/CKrnl_EditBox.cpp
        EAppControl/CKrnl_GroupBox.cpp
        EAppControl/CKrnl_HScrollBar.cpp
        EAppControl/CKrnl_Label.cpp
        EAppControl/CKrnl_ListBox.cpp
        EAppControl/CKrnl_PicBox.cpp
        EAppControl/CKrnl_PicBtn.cpp
        EAppControl/CKrnl_ProcessBar.cpp
        EAppControl/CKrnl_RadioBox.cpp
        EAppControl/CKrnl_ShapeBox.cpp
        EAppControl/CKrnl_SliderBar.cpp
        EAppControl/CKrnl_Tab.cpp
        EAppControl/CKrnl_Timer.cpp
        EAppControl/CKrnl_VScrollBar.cpp
        EAppControl/CIext2_CartoonBox.cpp
        EAppControl/CIext2_IPEditBox.cpp
        EAppControl/CIext2_RichEdit.cpp
        EAppControl/CIext2_SplitterBar.cpp
        EAppControl/CIext2_SuperAnimateBox.cpp
        EAppControl/CIext2_SuperBtn.cpp
        Module/CTreeFixer.cpp
        Module/EAppControlXref.cpp
        Module/ECSigMaker.cpp
        Module/ECSigScanner.cpp
        Module/MicroCodeFixer.cpp
        Module/ShowEventList.cpp
        Module/ShowImports.cpp
        Utils/Common.cpp
        Utils/IDAMenu.cpp
        Utils/IDAWrapper.cpp
        Utils/Strings.cpp
        Utils/md5.cpp
)

target_compile_definitions(E-Decompiler PRIVATE EDEC_HEXRAYS_API_VERSION=${EDEC_HEXRAYS_API_VERSION})
