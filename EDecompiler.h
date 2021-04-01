#pragma once
#include <pro.h>
#include "SectionManager.h"


enum EProgramsType_t
{
	E_UNKNOWN = 0,  //未知类型
	E_DYNAMIC,      //动态编译程序
	E_STATIC,       //静态编译程序
};

#pragma pack(push,1)

struct EVENT_INFO
{
	uint32 m_lpszName;       //事件名称
	uint32 m_lpszExplain;    //事件详细解释
	uint32 m_dwState;        //事件特殊标志
	int32  m_nArgCount;      //事件的参数数目
	uint32 m_lpEventArgInfo; //参数
};

// 用作定义窗口单元属性
struct UNIT_PROPERTY
{
	uint32 m_lpszName;      //属性名称
	uint32 m_lpszEGName;    //英文名称
	uint32 m_lpszExplain;   //属性解释
	int16  m_shtType;       //属性的数据类型
	uint16 m_wState;        //属性的特殊标记
	uint32 m_lpszzPickStr;  //备选文本
};

struct LIB_DATA_TYPE_INFO   //库定义数据类型结构
{
	uint32 m_lpszName;     //名称
	uint32 m_lpszEGName;   //英文名称,可为空
	uint32 m_szExplain;    //详细解释,可为空
	int32  m_nCmdCount;    //本数据类型成员方法的数目(可为0)
	uint32 m_lpnCmdsIndex; //指向所有成员方法命令在支持库命令表中的索引值指针,编译后数据被抹除
	uint32 m_dwState;      //数据类型的特殊属性

	 ////////////////////////////////////////////
	// 以下成员只有在为窗口单元、菜单时才有效。

	uint32 m_dwUnitBmpID;     //指定在支持库中的单元图像资源ID
	int32  m_nEventCount;     //本单元的事件数目
	uint32 m_lpEventBegin;    //指向单元的所有事件的指针,EVENT_INFO,编译后数据被抹除
	int32 m_nPropertyCount;   //本单元的属性数目
	uint32 m_lpPropertyBegin; //指向单元的所有属性的指针,UNIT_PROPERTY

	uint32 m_lpfnGetInterface; //用作提供本窗口单元的所有接口。

	////////////////////////////////////////////
	// 以下成员只有在不为窗口单元、菜单时才有效。

	int32 m_nElementCount;    //本数据类型中子成员的数目(可为0)
	uint32 m_lpElementBegin;   //指向子成员数组的首地址,LIB_DATA_TYPE_ELEMENT
};

struct LIB_INFO
{
	uint32 m_dwLibFormatVer;    //支持库格式版本号,应该为0x1312D65
	uint32 m_lpGuid;            //对应支持库的GUID指针。
	int32  m_nMajorVersion;     //支持库的主版本号，必须大于0。
	int32  m_nMinorVersion;     //支持库的次版本号。
	int32  m_nBuildNumber;      //构建版本号
	int32  m_nRqSysMajorVer;    //所需要易语言系统的主版本号
	int32  m_nRqSysMinorVer;    //所需要易语言系统的次版本号
	int32  m_nRqSysKrnlLibMajorVer;   //所需要的系统核心支持库的主版本号
	int32  m_nRqSysKrnlLibMinorVer;   //所需要的系统核心支持库的次版本号
	uint32 m_lpName;            //支持库名称指针
	int32  m_nLanguage;         //支持库所支持的语言,应该是1
	uint32 m_lpExplain;         //支持库解释内容指针,可为空
	uint32 m_dwState;           //支持库特殊状态说明
	uint32 m_lpszAuthor;        //作者相关信息
	uint32 m_lpszZipCode;       //作者相关信息
	uint32 m_lpszAddress;       //作者相关信息
	uint32 m_lpszPhone;         //作者相关信息
	uint32 m_lpszFax;           //作者相关信息
	uint32 m_lpszEmail;         //作者相关信息
	uint32 m_lpszHomePage;      //作者相关信息
	uint32 m_lpszOther;         //作者相关信息

//////////////////
	int32 m_nDataTypeCount;     //支持库全部的数据类型个数
	uint32 m_lpDataType;        //指向程序用到的数据类型信息的指针,LIB_DATA_TYPE_INFO

	int32 m_nCategoryCount;     //全局命令类别数目
	uint32 m_lpszzCategory;     //全局命令类别说明表，每项为一字符串，前四位数字表示图象索引号（从1开始，0表示无）。
		                        // 减一后的值为指向支持库中名为"LIB_BITMAP"的BITMAP资源中某一部分16X13位图的索引

	int32 m_nCmdCount;          //本库中提供的所有命令(全局命令及对象方法)的数目(如无则为0)。
	uint32 m_lpBeginCmdInfo;    //指向所有命令及方法的定义信息数组(如m_nCmdCount为0,则为NULL),CMD_INFO
	uint32 m_lpCmdsFunc;        //指向每个命令的实现代码首地址，(如m_nCmdCount为0, 则为NULL)。

	uint32 m_lpfnRunAddInFn;    //可为NULL，用作为易语言IDE提供附加功能
	uint32 m_szzAddInFnInfo;    //有关AddIn功能的说明，两个字符串说明一个功能

	uint32 m_lpfnNotify;        //不能为NULL，提供接收来自易语言IDE或运行环境通知信息的函数。

	// 超级模板暂时保留不用。
	uint32 m_lpfnSuperTemplate;       //为空
	uint32 m_lpszzSuperTemplateInfo;  //为空

	// 本库定义的所有常量。
	int32 m_nLibConstCount;   //常量数据
	uint32 m_lpLibConst;      //指向常量定义数组的指针

	uint32 m_lpszzDependFiles; //本库正常运行所需要依赖的其他文件，在制作安装软件时将会自动带上这些文件,可为空
};

struct EHead
{
	uint32 dwMagic;  //未知,值固定为3
	uint32 szNone2;  //未知,值固定为0
	uint32 szNone3;  //未知,好像是个随机数,修改不影响程序
	uint32 lpStartCode;   //起始用户代码地址,不可修改
	uint32 lpEString;     //字符串资源,如果没有字符串资源,则为0
	uint32 dwEStringSize; //字符串资源大小,如果没有字符串资源,则为0
	uint32 lpEWindow;     //创建组件信息
	uint32 dwEWindowSize; //创建组件信息大小
	uint32 dwLibNum;      //支持库数量
	uint32 lpLibEntry;    //支持库信息入口
	uint32 dwApiCount;    //Api数量
	uint32 lpModuleName;  //指向模块名称
	uint32 lpApiName;     //指向Api名称
};

#pragma pack(pop)


//下面以mid开头的是自定义的存储信息结构体
enum BinType_t
{
	e_UnknownValue = 0,
	e_NullBin,
	e_BinValue,
	e_NullStr,
	e_StringValue,
	e_ArrayHead,
	e_FloatValue,
	e_ClassTable,
	e_SwitchTable,         //switch case表
};

struct mid_BinSource
{
	BinType_t itype;
	ea_t address;
	unsigned int extraData;	//附加数据
};

struct mid_EDataTypeInfo
{
	qstring m_Name;  //数据类型名称
};

struct mid_EventInfo
{
	int m_nEventIndex;       //事件索引
	uint32 m_EventAddr;      //事件地址
};

//控件的基本属性
struct mid_EBasicProperty
{
	qstring m_controlName;   //名称
	int m_left;              //左边
	int m_top;               //顶边
	int m_width;             //宽度
	int m_height;            //高度
	qstring m_tag;           //标记
	qvector<mid_EventInfo> mVec_eventInfo;   //事件处理
};

struct mid_ELibInfo
{
	qstring m_Name;          //支持库名称
	qstring m_Guid;          //支持库的GUID
	int32  m_nMajorVersion;  //支持库的主版本号，必须大于0。
	int32  m_nMinorVersion;  //支持库的次版本号。

	qvector<mid_EDataTypeInfo> mVec_DataTypeInfo;      //数据类型信息
};

struct mid_ControlInfo
{
	uint32 m_controlId;                 //控件自身ID
	uint32 m_controlTypeId;             //控件所属数据类型的ID
	qstring m_controlTypeName;          //控件类型名称
	bool b_isMenu;                      //是否为菜单控件
	mid_EBasicProperty m_basicProperty; //控件的基础属性
};

struct mid_GuiInfo
{
	uint32 m_windowId;                           //控件所属窗口ID
	qvector<mid_ControlInfo> mVec_ControlInfo;   //窗口中的控件
};

struct mid_KrnlApp
{
	ea_t krnl_MReportError;               //错误回调
	ea_t krnl_MCallDllCmd;                //DLL命令
	ea_t krnl_MCallLibCmd;                //三方支持库命令
	ea_t krnl_MCallKrnlLibCmd;            //核心支持库命令
	ea_t krnl_MReadProperty;              //读取组件属性
	ea_t krnl_MWriteProperty;             //设置组件属性
	ea_t krnl_MMalloc;                    //分配内存
	ea_t krnl_MRealloc;                   //重新分配内存
	ea_t krnl_MFree;                      //释放内存
	ea_t krnl_MExitProcess;               //结束
	ea_t krnl_MMessageLoop;               //窗口消息循环
	ea_t krnl_MLoadBeginWin;              //载入启动窗口
	ea_t krnl_MOtherHelp;                 //辅助功能
};

struct mid_EAppInfo
{
	ea_t m_UserCodeStartAddr;                          //用户起始地址
	ea_t m_UserCodeEndAddr;                            //用户结束地址,目前暂时还没有什么好办法获取这个地址,如果有好的想法欢迎提issue
	qvector<mid_BinSource> mVec_UserResource;          //用户资源
	qvector<mid_GuiInfo>   mVec_GuiInfo;               //控件信息
	qvector<mid_ELibInfo>  mVec_LibInfo;               //支持库信息

	mid_KrnlApp m_KrnlApp;
	bool b_IsWindowProgram;                            //是否是窗体程序
};

class IDAMenu;
class EDecompilerEngine:public SectionManager
{
public:
	EDecompilerEngine();
	~EDecompilerEngine();

public:
	bool InitDecompilerEngine();
	bool DoDecompile();
private:
	bool DoDecompiler_EStatic();
	//根据交叉引用来判断指定地址的数据类型
	BinType_t GetBinValueType(ea_t addr);
	//根据菜单的类型ID来得到名称
	qstring GetLibDataTypeInfo(uint32 typeId);
	//解析界面控件信息
	bool ParseGUIResource(ea_t, uint32);
	//解析常量资源信息
	bool ParseStringResource(ea_t, uint32);
	//解析支持库信息
	bool ParseLibInfomation(ea_t, uint32);
	//解析系统接口函数
	bool ParseKrnlInterface(ea_t);
	//是否为菜单项
	static bool krnln_IsMenuItemID(unsigned int ID);
	//解析窗体资源属性
	void Parse_MainWindow(unsigned char* lpControlInfo, mid_EBasicProperty& out_Property);
public:
	EProgramsType_t m_ProgramType;
	mid_EAppInfo m_eAppInfo;
private:
	ea_t m_EHeadAddr;
	IDAMenu* gMenu_ShowResource = nullptr;
	IDAMenu* gMenu_ShowGUIInfo = nullptr;
};

extern EDecompilerEngine g_MyDecompiler;