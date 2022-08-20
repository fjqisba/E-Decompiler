#pragma once
#include <pro.h>
#include <QMap>
#include <QVariant>
#include "SectionManager.h"
#include "ESymbol.h"
#include "./Module/CTreeFixer.h"

enum EArchitectureType
{
	E_UNKNOWN = 0,  //未知类型
	E_DYNAMIC,      //动态编译程序
	E_STATIC,       //静态编译程序
};

struct mid_EAppInfo
{
	bool b_IsWindowProgram;                            //是否是窗体程序
	unsigned int m_EventSum;                           //所有的控件事件个数
};

class IDAMenu;

class EDecompiler
{
public:
	EDecompiler();
	~EDecompiler();
public:
	//单例模式
	static EDecompiler& Instance();

	//初始化易语言反编译引擎
	bool InitDecompilerEngine();

	bool DoDecompile();


	static ssize_t ui_callback(void* ud, int notification_code, va_list va);
private:
	//扫描易语言函数
	void makeFunction(ea_t startAddr, ea_t endAddr);

	//探测易语言程序类型
	bool InitEArchitectureType();

	bool Parse_EStatic(unsigned int eHeadAddr);


	

	//修复易语言声明
	void FixEStructure();
public:
	EArchitectureType arch;
	ESymbol eSymbol;
	CTreeFixer cTreeFixer;
	mid_EAppInfo m_eAppInfo;
private:

	IDAMenu* gMenu_ShowResource = nullptr;
	IDAMenu* gMenu_ShowGUIInfo = nullptr;
	IDAMenu* gMenu_ShowEventInfo = nullptr;
	IDAMenu* gMenu_ShowImportsInfo = nullptr;
};