#pragma once
#include <pro.h>
#include <QVariant>

#define COLUMN_PropertyName  0
#define COLUMN_PropertyValue 1

enum ControlType_t
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
};

class ControlInfoWidget;
class EAppControl
{
public:
	//获取控件的附加单元数据
	static unsigned char* GetUnitDataPtr(unsigned char* propertyAddr);
	//解析每个控件都有的基础属性
	static void 解析控件基础属性(unsigned char* lpControlInfo, QHash<QString, QVariant>& out_data);

	static void 显示控件信息(ControlType_t type, unsigned int propertyAddr, int propertySize);
	static void 添加文本控件(QString PropertyName, QString PropertyValue);
	static void 添加布尔控件(QString PropertyName, bool ProperyValue);
	static void 添加列表控件(QString PropertyName, QStringList& PropertyList, unsigned int PropertyValue);
	static void 添加鼠标控件(unsigned int ProperyValue);
	static void 添加图片控件(QString PropertyName, QByteArray& PropertyValue);
	static void 添加颜色控件(QString PropertyName, unsigned int PropertyValue);
	static void 添加字体控件(QString PropertyName, QByteArray& PropertyValue);
	static void 添加无效控件(QString PropertyName, QString PropertyValue);

	static void 添加底色控件(QString propertyName, unsigned int ProperyValue);
public:
	virtual void 取控件默认附加属性(QHash<QString, QVariant>& out_data) = 0;
	virtual void 反序列化控件附加属性(unsigned char* pUnitDataPtr, QHash<QString, QVariant>& out_data) = 0;
	virtual void 显示控件属性信息(QHash<QString, QVariant>& out_data) = 0;
	virtual qstring 取事件名称(int eventIndex) = 0;
};