#pragma once

struct eSymbol_EventInfo
{
	int eventIndex;          //事件索引
	unsigned int eventAddr;  //事件地址
};

struct eSymbol_ImportsApi
{
	std::string libName;
	std::string apiName;
};

enum eSymbol_ControlType
{
	UnknownControl = 0,
	krnl_window,     //窗口
	krnl_menu,       //菜单
	krnl_EditBox,    //编辑框
	krnl_PicBox,     //图片框
	krnl_ShapeBox,   //外形框
	krnl_DrawPanel,  //画板
	krnl_GroupBox,   //分组框
	krnl_Label,      //标签
	krnl_Button,     //按钮
	krnl_CheckBox,   //选择框
	krnl_RadioBox,   //单选框
	krnl_ComboBox,   //组合框
	krnl_ListBox,    //列表框
	krnl_ChkListBox, //选择列表框
	krnl_HScrollBar, //横向滚动条
	krnl_VScrollBar, //纵向滚动条
	krnl_ProcessBar, //进度条
	krnl_SliderBar,  //滑块条
	krnl_Tab,        //选择夹
	krnl_AnimateBox, //影像框
	krnl_DatePicker, //日期框
	krnl_MonthCalendar,  //月历
	krnl_DriverBox,  //驱动器框
	krnl_DirBox,     //目录框
	krnl_FileBox,    //文件框
	krnl_ColorPicker, //颜色选择器
	krnl_HyperLinker, //超级链接器
	krnl_Spin,        //调节器
	krnl_CommonDlg,   //通用对话框
	krnl_Timer,       //时钟
	krnl_printer,     //打印机
	krnl_UDP,         //数据报
	krnl_Client,      //客户
	krnl_Server,      //服务器
	krnl_SerialPort,  //端口
	krnl_Grid,        //表格
	krnl_DataSrc,     //数据源
	krnl_NProvider,   //通用提供者
	krnl_DBProvider,  //数据库提供者
	krnl_PicBtn,      //图形按钮
	krnl_ODBCDB,      //外部数据库
	krnl_ODBCProvider,//外部数据提供者
	krnl_DropTarget,  //拖放对象


	iext2_CartoonBox,
	iext2_IPEditBox,
	iext2_RichEdit,
	iext2_SplitterBar,
	iext2_SuperAnimateBox,
	iext2_SuperBtn,
};