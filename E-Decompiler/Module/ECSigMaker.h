#pragma once
#include <pro.h>
#include <kernwin.hpp>

//制作易语言函数特征

class ESymbol;
class ECSigMaker:public action_handler_t
{
public:
	ECSigMaker(ESymbol& symbol);
	//注册Handler
	void RegisterAction(void* owner);
	//添加菜单项
	void AttachToPopupMenu(TWidget* view, TPopupMenu* p);
private:
	int activate(action_activation_ctx_t* ctx)override;
	action_state_t idaapi update(action_update_ctx_t* ctx) override;
	ESymbol& eSymbol;
};