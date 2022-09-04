#pragma once
#include <pro.h>
#include <idp.hpp>
#include "SectionManager.h"
#include "ESymbol.h"
#include "./Module/CTreeFixer.h"
#include "./Module/ECSigMaker.h"
#include "./Module/EAppControlXref.h"
#include "./Module/MicroCodeFixer.h"

enum EArchitectureType
{
	E_UNKNOWN = 0,  //未知类型
	E_DYNAMIC,      //动态编译程序
	E_STATIC,       //静态编译程序
};

class IDAMenu;

class EDecompiler:public plugmod_t
{
public:
	EDecompiler();
	~EDecompiler();
public:
	bool idaapi run(size_t) override;
	//初始化易语言反编译引擎
	bool InitDecompilerEngine();
private:
	//扫描易语言函数
	void makeFunction(ea_t startAddr, ea_t endAddr);

	//探测易语言程序类型
	bool initEArchitectureType();

	bool Parse_EStatic(unsigned int eHeadAddr);

	//导入易语言声明
	void ImportsEStructure();
public:
	EArchitectureType arch;
	ESymbol eSymbol;
	CTreeFixer cTreeFixer;

	ECSigMaker ecSigMaker;
	EAppControlXref eControlXref;
private:
	IDAMenu* gMenu_ShowResource = nullptr;
	IDAMenu* gMenu_ShowGUIInfo = nullptr;
	IDAMenu* gMenu_ShowEventInfo = nullptr;
	IDAMenu* gMenu_ShowImportsInfo = nullptr;
};