#pragma once
#include "EAppControl.h"

//IP±à¼­¿ò

struct CIext2_IPEditBox :public EAppControl
{
public:
	CIext2_IPEditBox();
	static CIext2_IPEditBox* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};