#pragma once
#include <pro.h>

enum EProgramsType_t
{
	E_UNKNOWN = 0,  //未知类型
	E_DYNAMIC,      //动态编译程序
	E_STATIC,       //静态编译程序
};

#pragma pack(push,1)

struct EHead
{
	uint32 dwMagic;  //未知
	uint32 szNone2;  //未知
	uint32 szNone3;  //未知
	uint32 lpStartCode;   //起始程序地址
	uint32 lpEString;     //字符串资源,如果没有字符串资源,则为0
	uint32 dwEStringSize; //字符串资源大小,如果没有字符串资源,则为0
	uint32 lpEWindow;     //创建组件信息
	uint32 dwEWindowSize; //创建组件信息大小
	uint32 dwLibNum;      //支持库数量
	uint32 lpLibEntry;    //支持库信息入口
	uint32 dwApiCount;    //Api数量
	uint32 lpLibName;     //指向支持库名称
	uint32 lpApiName;     //指向Api名称
};

#pragma pack(pop)


struct mid_EHeadInfo
{
	ea_t m_UserCodeStartAddr;                          //用户起始地址
	qvector<qstring> m_VecStringResource;              //字符串资源
	qvector<qvector<unsigned char>> m_VecBinResource;  //字节集资源
};

class EDecompilerEngine
{
public:
	EDecompilerEngine();
	~EDecompilerEngine();
public:
	bool DoDecompiler();
private:
	bool DoDecompiler_EStatic();
	bool ParseStringResource(ea_t, uint32);
public:
	EProgramsType_t m_ProgramType;
private:
	ea_t m_EHeadAddr;
	mid_EHeadInfo m_eHeadInfo;
};