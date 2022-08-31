#pragma once
#include "EAppControl.h"

//·Ö¸ôÌõ

struct CIext2_SplitterBar :public EAppControl
{
public:
	CIext2_SplitterBar();
	static CIext2_SplitterBar* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};