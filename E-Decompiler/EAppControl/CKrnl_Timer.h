#pragma once
#include "EAppControl.h"

// ±÷”

struct CKrnl_Timer :public EAppControl
{
public:
	CKrnl_Timer();
	static CKrnl_Timer* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};