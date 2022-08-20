#pragma once
#include "EAppControl.h"

//»­°å

struct CKrnl_DrawPanel :public EAppControl
{
public:
	CKrnl_DrawPanel();
	static CKrnl_DrawPanel* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};