#pragma once
#include "EAppControl.h"

//·Ö×é¿ò

struct CKrnl_GroupBox :public EAppControl
{
public:
	CKrnl_GroupBox();
	static CKrnl_GroupBox* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};