#pragma once
#include "EAppControl.h"

//Ñ¡Ôñ¼Ð


struct CKrnl_Tab :public EAppControl
{
public:
	CKrnl_Tab();
	static CKrnl_Tab* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};