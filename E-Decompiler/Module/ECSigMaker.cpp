#include "ECSigMaker.h"
#include "../Utils/Strings.h"
#include "ECSigScanner.h"
#include <funcs.hpp>

#define ACTION_ECSIGMAKER "EDecompiler::GenerateECSig"


ECSigMaker::ECSigMaker(ESymbol& symbol):eSymbol(symbol)
{

}

void ECSigMaker::RegisterAction(void* owner)
{
	//注册窗口菜单
	std::string menuName = LocalCpToUtf8("生成易语言函数特征");
	const action_desc_t GenEsigDesc = {
	sizeof(action_desc_t),ACTION_ECSIGMAKER,menuName.c_str(),this,
	owner,nullptr,nullptr,0,ADF_OT_PLUGMOD};
	register_action(GenEsigDesc);
}

void ECSigMaker::AttachToPopupMenu(TWidget* view, TPopupMenu* p)
{
	attach_action_to_popup(view,p, ACTION_ECSIGMAKER,nullptr, SETMENU_FIRST);
}

int ECSigMaker::activate(action_activation_ctx_t* ctx)
{
	unsigned int curEA = get_screen_ea();
	func_t* pFunc = get_func(curEA);
	if (!pFunc) {
		return 0;
	}
	std::string goodMD5 = ECSigScanner::Instance().GetFunctionMD5(pFunc->start_ea,false);


	return 0;
}

action_state_t idaapi ECSigMaker::update(action_update_ctx_t* ctx)
{
	return AST_ENABLE_ALWAYS;
}
