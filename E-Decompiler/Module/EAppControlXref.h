#pragma once
#include <pro.h>
#include <kernwin.hpp>
#include <vector>

//易语言控件交叉引用

class ESymbol;
struct EAppControl;

class EAppControlXref:public action_handler_t
{
	//交叉引用类型
	enum eAppControlXrefType
	{
		XrefWriteProperty = 0x0,
		XrefReadProperty = 0x1,
		XrefExecute = 0x2,
	};

	struct eAppControlXrefData
	{
		eAppControlXrefType type;
		unsigned int xRefAddr;
		std::string text;
	};
public:
	EAppControlXref(ESymbol& symbol);
	//注册Handler
	void RegisterAction(void* owner);
	//添加菜单项
	void AttachToPopupMenu(TWidget* view, TPopupMenu* p);
private:
	//初始化交叉引用数据
	bool initXrefData(unsigned int callAddr);
	int activate(action_activation_ctx_t* ctx)override;
	action_state_t idaapi update(action_update_ctx_t* ctx) override;
	//获取读写交叉引用地址的控件信息,返回false表示失败
	bool getRWXrefAddrText(unsigned int XrefAddr, std::string& outText);
	//获取执行交叉引用地址的控件信息,返回false表示失败
	bool getExecuteXrefAddrText(unsigned int XrefAddr, std::string& outText);
	void showXrefList();
private:
	ESymbol& symbolTable;
	std::vector<eAppControlXrefData> allXrefData;
	EAppControl* currentAppControl;
	unsigned int currentAddr;
	unsigned int windowID;
	unsigned int controlID;
};