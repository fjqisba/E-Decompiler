#pragma once
#include <pro.h>
#include <kernwin.hpp>

//���������Ժ�������

class ECSigMaker:public action_handler_t
{
public:
	//ע��Handler
	void RegisterAction(void* owner);
	//���Ӳ˵���
	void AttachToPopupMenu(TWidget* view, TPopupMenu* p);
private:
	int activate(action_activation_ctx_t* ctx)override;
	action_state_t idaapi update(action_update_ctx_t* ctx) override;
};