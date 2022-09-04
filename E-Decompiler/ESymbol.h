#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include "ELib.h"

//易语言符号信息

struct EVENT_INFO
{
	unsigned int m_lpszName;       //事件名称
	unsigned int m_lpszExplain;    //事件详细解释
	unsigned int m_dwState;        //事件特殊标志
	int  m_nArgCount;      //事件的参数数目
	unsigned int m_lpEventArgInfo; //参数
};

#pragma pack(push,1)



// 用作定义窗口单元属性
struct UNIT_PROPERTY
{
	unsigned int m_lpszName;      //属性名称
	unsigned int m_lpszEGName;    //英文名称
	unsigned int m_lpszExplain;   //属性解释
	short  m_shtType;       //属性的数据类型
	unsigned short m_wState;        //属性的特殊标记
	unsigned int m_lpszzPickStr;  //备选文本
};

struct LIB_DATA_TYPE_INFO   //库定义数据类型结构
{
	unsigned int m_lpszName;     //名称
	unsigned int m_lpszEGName;   //英文名称,可为空
	unsigned int m_szExplain;    //详细解释,可为空
	int  m_nCmdCount;    //本数据类型成员方法的数目(可为0)
	unsigned int m_lpnCmdsIndex; //指向所有成员方法命令在支持库命令表中的索引值指针,编译后数据被抹除
	unsigned int m_dwState;      //数据类型的特殊属性

	 ////////////////////////////////////////////
	// 以下成员只有在为窗口单元、菜单时才有效。

	unsigned int m_dwUnitBmpID;     //指定在支持库中的单元图像资源ID
	int  m_nEventCount;     //本单元的事件数目
	unsigned int m_lpEventBegin;    //指向单元的所有事件的指针,EVENT_INFO,编译后数据被抹除
	int m_nPropertyCount;   //本单元的属性数目
	unsigned int m_lpPropertyBegin; //指向单元的所有属性的指针,UNIT_PROPERTY

	unsigned int m_lpfnGetInterface; //用作提供本窗口单元的所有接口。

	////////////////////////////////////////////
	// 以下成员只有在不为窗口单元、菜单时才有效。

	int m_nElementCount;    //本数据类型中子成员的数目(可为0)
	unsigned int m_lpElementBegin;   //指向子成员数组的首地址,LIB_DATA_TYPE_ELEMENT
};

struct LIB_INFO
{
	unsigned int m_dwLibFormatVer;    //支持库格式版本号,应该为0x1312D65
	unsigned int m_lpGuid;            //对应支持库的GUID指针。
	int  m_nMajorVersion;     //支持库的主版本号，必须大于0。
	int  m_nMinorVersion;     //支持库的次版本号。
	int  m_nBuildNumber;      //构建版本号
	int  m_nRqSysMajorVer;    //所需要易语言系统的主版本号
	int  m_nRqSysMinorVer;    //所需要易语言系统的次版本号
	int  m_nRqSysKrnlLibMajorVer;   //所需要的系统核心支持库的主版本号
	int  m_nRqSysKrnlLibMinorVer;   //所需要的系统核心支持库的次版本号
	unsigned int m_lpName;            //支持库名称指针
	int  m_nLanguage;         //支持库所支持的语言,应该是1
	unsigned int m_lpExplain;         //支持库解释内容指针,可为空
	unsigned int m_dwState;           //支持库特殊状态说明
	unsigned int m_lpszAuthor;        //作者相关信息
	unsigned int m_lpszZipCode;       //作者相关信息
	unsigned int m_lpszAddress;       //作者相关信息
	unsigned int m_lpszPhone;         //作者相关信息
	unsigned int m_lpszFax;           //作者相关信息
	unsigned int m_lpszEmail;         //作者相关信息
	unsigned int m_lpszHomePage;      //作者相关信息
	unsigned int m_lpszOther;         //作者相关信息

//////////////////
	int m_nDataTypeCount;     //支持库全部的数据类型个数
	unsigned int m_lpDataType;        //指向程序用到的数据类型信息的指针,LIB_DATA_TYPE_INFO

	int m_nCategoryCount;     //全局命令类别数目
	unsigned int m_lpszzCategory;     //全局命令类别说明表，每项为一字符串，前四位数字表示图象索引号（从1开始，0表示无）。
								// 减一后的值为指向支持库中名为"LIB_BITMAP"的BITMAP资源中某一部分16X13位图的索引

	int m_nCmdCount;          //本库中提供的所有命令(全局命令及对象方法)的数目(如无则为0)。
	unsigned int m_lpBeginCmdInfo;    //指向所有命令及方法的定义信息数组(如m_nCmdCount为0,则为NULL),CMD_INFO
	unsigned int m_lpCmdsFunc;        //指向每个命令的实现代码首地址，(如m_nCmdCount为0, 则为NULL)。

	unsigned int m_lpfnRunAddInFn;    //可为NULL，用作为易语言IDE提供附加功能
	unsigned int m_szzAddInFnInfo;    //有关AddIn功能的说明，两个字符串说明一个功能

	unsigned int m_lpfnNotify;        //不能为NULL，提供接收来自易语言IDE或运行环境通知信息的函数。

	// 超级模板暂时保留不用。
	unsigned int m_lpfnSuperTemplate;       //为空
	unsigned int m_lpszzSuperTemplateInfo;  //为空

	// 本库定义的所有常量。
	int m_nLibConstCount;   //常量数据
	unsigned int m_lpLibConst;      //指向常量定义数组的指针

	unsigned int m_lpszzDependFiles; //本库正常运行所需要依赖的其他文件，在制作安装软件时将会自动带上这些文件,可为空
};

#pragma pack(pop)



struct eSymbol_EDataTypeInfo
{
	std::string m_Name;  //数据类型名称
};

struct eSymbol_ELibInfo
{
	std::string m_Name;         //支持库名称
	std::string m_Guid;         //支持库的GUID
	int  m_nMajorVersion;       //支持库的主版本号，必须大于0。
	int  m_nMinorVersion;       //支持库的次版本号。

	std::vector<eSymbol_EDataTypeInfo> mVec_DataTypeInfo;      //数据类型信息
};

struct eSymbol_KrnlCall
{
	unsigned int krnl_MReportError;               //错误回调
	unsigned int krnl_MCallDllCmd;                //DLL命令
	unsigned int krnl_MCallLibCmd;                //三方支持库命令
	unsigned int krnl_MCallKrnlLibCmd;            //核心支持库命令
	unsigned int krnl_MReadProperty;              //读取组件属性
	unsigned int krnl_MWriteProperty;             //设置组件属性
	unsigned int krnl_MMalloc;                    //分配内存
	unsigned int krnl_MRealloc;                   //重新分配内存
	unsigned int krnl_MFree;                      //释放内存
	unsigned int krnl_MExitProcess;               //结束
	unsigned int krnl_MMessageLoop;               //窗口消息循环
	unsigned int krnl_MLoadBeginWin;              //载入启动窗口
	unsigned int krnl_MOtherHelp;                 //辅助功能
};

struct eSymbol_KrnlJmp
{
	unsigned int Jmp_MReportError;               //错误回调
	unsigned int Jmp_MCallDllCmd;                //DLL命令
	unsigned int Jmp_MCallLibCmd;                //三方支持库命令
	unsigned int Jmp_MCallKrnlLibCmd;            //核心支持库命令
	unsigned int Jmp_MReadProperty;              //读取组件属性
	unsigned int Jmp_MWriteProperty;             //设置组件属性
	unsigned int Jmp_MMalloc;                    //分配内存
	unsigned int Jmp_MRealloc;                   //重新分配内存
	unsigned int Jmp_MFree;                      //释放内存
	unsigned int Jmp_MExitProcess;               //结束
	unsigned int Jmp_MMessageLoop;               //窗口消息循环
	unsigned int Jmp_MLoadBeginWin;              //载入启动窗口
	unsigned int Jmp_MOtherHelp;                 //辅助功能
};

struct EComHead
{
	unsigned int dwMagic;  //未知,值固定为3
	unsigned int szNone2;  //未知,值固定为0
	unsigned int szNone3;  //未知,好像是个随机数,修改不影响程序
	unsigned int lpStartCode;   //起始用户代码地址,不可修改
	unsigned int lpEString;     //字符串资源,如果没有字符串资源,则为0
	unsigned int dwEStringSize; //字符串资源大小,如果没有字符串资源,则为0
	unsigned int lpEWindow;     //创建组件信息
	unsigned int dwEWindowSize; //创建组件信息大小
	unsigned int dwLibNum;      //支持库数量
	unsigned int lpLibEntry;    //支持库信息入口
	unsigned int dwApiCount;    //Api数量
	unsigned int lpModuleName;  //指向模块名称
	unsigned int lpApiName;     //指向Api名称
};

enum eSymbolFuncType
{
	eFunc_Unknown = 0x0,
	//易语言库函数
	eFunc_KrnlLibFunc,
	//读取组件属性
	eFunc_KrnlReadProerty,
	//设置组件属性
	eFunc_KrnlWriteProperty,
	//调用DLL命令
	eFunc_KrnlDllCmd,
	//错误回调
	eFunc_KrnlReportError,
	//释放内存
	eFunc_KrnlFreeMem,
	//文本相加
	eFunc_Strcat,
	//连续省略参数
	eFunc_PushDefaultArg,
	//计算多维数组下标
	eFunc_CalMultiArrayIndex,
};

struct EAppControl;

struct eSymbol_ControlIndex
{
	unsigned int windowId;
	unsigned int controlId;
public:
	eSymbol_ControlIndex() {};
	eSymbol_ControlIndex(unsigned int w, unsigned int c)
	{
		windowId = w;
		controlId = c;
	}
	bool operator< (const eSymbol_ControlIndex& com)const
	{
		if (com.windowId != this->windowId) {
			return com.windowId < this->windowId;
		}
		return com.controlId < this->controlId;
	}
};


class ESymbol
{
public:
	ESymbol();
	~ESymbol();
public:
	//加载易语言静态编译符号
	bool LoadEStaticSymbol(unsigned int eHeadAddr, EComHead* eHead);

	//获取函数符号类型
	eSymbolFuncType GetFuncSymbolType(unsigned int addr);
	//通过窗口ID和控件ID索引到控件
	EAppControl* GetEAppControl(unsigned int windowID,unsigned int controID);
private:
	//加载支持库信息
	bool loadELibInfomation(unsigned int lpLibStartAddr, unsigned int dwLibCount);
	//扫描并识别支持库函数
	bool scanELibFunction(unsigned int lpLibStartAddr, unsigned int dwLibCount);
	//扫描并识别易语言基础命令
	bool scanBasicFunction();
	//加载易语言核心函数
	bool loadKrnlInterface(unsigned int lpKrnlEntry);
	//加载界面资源信息
	bool loadGUIResource(unsigned int lpGUIStart, unsigned int infoSize);
	//加载用户导入表
	bool loadUserImports(unsigned int dwApiCount, unsigned int lpModuleName, unsigned int lpApiName);
	//处理连续省略参数
	bool handleFuncPushDefaultArg(unsigned int callAddr);
	//扫描易语言类虚表
	bool scanEClassTable();
	//解析控件基础属性
	void parseControlBasciProperty(unsigned char* lpControlInfo, EAppControl* outControl);
	//根据菜单的类型ID来得到名称,0x10001 -> 窗口
	std::string getControlTypeName(unsigned int typeId);
	
	bool registerKrnlJmpAddr(unsigned int callAddr, unsigned int setAddr);

	//设置控件的事件名称
	void setGuiEventName();
	//清理控件
	void clearControlData();
public:
	//支持库信息
	std::vector<eSymbol_ELibInfo> vec_ELibInfo;
	//核心函数
	eSymbol_KrnlCall krnlCall;
	eSymbol_KrnlJmp krnlJmp;
	//用户起始地址
	unsigned int userCodeStartAddr;
	//用户结束地址,目前暂时还没有什么好办法获取这个地址,如果有好的想法欢迎提issue
	unsigned int userCodeEndAddr;

	//导入表,key是索引,value是导入信息
	std::vector<eSymbol_ImportsApi> importsApiList;
	//临时导入表
	std::vector<std::string> tmpImportsApiList;

	//存储所有的控件信息
	std::vector<EAppControl*> allControlList;
private:

	//标记函数类型
	std::map<unsigned int,eSymbolFuncType> eSymbolFuncTypeMap;
	//存储所有的控件信息,内容和allControlList一样
	std::map<eSymbol_ControlIndex, EAppControl*> allControlMap;
};