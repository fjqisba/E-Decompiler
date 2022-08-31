#pragma once
#include "EAppControl.h"

//¶¯»­¿ò

struct CIext2_CartoonBox :public EAppControl
{
public:
	CIext2_CartoonBox();
	static CIext2_CartoonBox* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};