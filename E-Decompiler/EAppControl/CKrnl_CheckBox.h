#pragma once
#include "EAppControl.h"

//Ñ¡Ôñ¿ò

struct CKrnl_CheckBox :public EAppControl
{
public:
	CKrnl_CheckBox();
	static CKrnl_CheckBox* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};