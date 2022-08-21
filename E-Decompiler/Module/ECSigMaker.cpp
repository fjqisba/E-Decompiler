#include "ECSigMaker.h"
#include "../Utils/Strings.h"

#define ACTION_ECSIGMAKER "EDecompiler::GenerateECSig"


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
	int a = 0;
	return 0;
}

action_state_t idaapi ECSigMaker::update(action_update_ctx_t* ctx)
{
	return AST_ENABLE_ALWAYS;
}
