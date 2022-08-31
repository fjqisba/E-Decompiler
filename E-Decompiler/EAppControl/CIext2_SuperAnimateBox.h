#pragma once
#include "EAppControl.h"

//¸ß¼¶Ó°Ïñ¿ò

struct CIext2_SuperAnimateBox :public EAppControl
{
public:
	CIext2_SuperAnimateBox();
	static CIext2_SuperAnimateBox* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};