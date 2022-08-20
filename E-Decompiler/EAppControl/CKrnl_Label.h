#pragma once
#include "EAppControl.h"

//±Í«©

struct CKrnl_Label :public EAppControl
{
public:
	CKrnl_Label();
	static CKrnl_Label* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};