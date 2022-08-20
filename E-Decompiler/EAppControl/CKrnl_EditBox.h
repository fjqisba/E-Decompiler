#pragma once
#include "EAppControl.h"

//±à¼­¿ò

struct CKrnl_EditBox :public EAppControl
{
public:
	CKrnl_EditBox();
	static CKrnl_EditBox* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};