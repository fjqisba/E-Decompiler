#pragma once
#include "EAppControl.h"


//×éºÏ¿ò

struct CKrnl_ComboBox :public EAppControl
{
public:
	CKrnl_ComboBox();
	static CKrnl_ComboBox* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};