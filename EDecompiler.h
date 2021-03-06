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

struct EHead
{
	uint32 dwMagic;  //未知,值固定为3
	uint32 szNone2;  //未知,值固定为0
	uint32 szNone3;  //未知,好像是个随机数,修改不影响程序
	uint32 lpStartCode;   //起始程序地址,不可修改
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

struct BinSource
{
	//-1表示是数组头
	//0表示是字符串
    //1表示是字节集
    //2表示是浮点数
	int itype;

	ea_t binAddr;
	qvector<unsigned char> binData;
	double floatData;
};

struct mid_EAppInfo
{
	ea_t m_UserCodeStartAddr;                          //用户起始地址
	ea_t m_UserCodeEndAddr;                            //用户结束地址,目前暂时还没有什么好办法获取这个地址,如果有好的想法欢迎提issue
	qvector<BinSource> m_VecUserResource;              //用户资源
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

	//判断指定地址的数据是否为易语言浮点常量
	bool IsFloatConstValue(ea_t);
	//判断指定地址的数据是否为易语言数组头
	bool IsArrayHeadValue(ea_t);
	//解析界面控件信息
	bool ParseGUIResource(ea_t, uint32);
	//解析常量资源信息
	bool ParseStringResource(ea_t, uint32);
	//解析支持库信息
	bool ParseLibInfomation(ea_t, uint32);
public:
	EProgramsType_t m_ProgramType;
	mid_EAppInfo m_eAppInfo;
private:
	ea_t m_EHeadAddr;
	IDAMenu* gMenu_ShowResource = nullptr;
};

extern EDecompilerEngine g_MyDecompiler;