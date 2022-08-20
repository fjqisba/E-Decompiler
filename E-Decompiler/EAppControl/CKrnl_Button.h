#pragma once
#include "EAppControl.h"

//°´Å¥

struct CKrnl_Button :public EAppControl
{
public:
	CKrnl_Button();
	static CKrnl_Button* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};
