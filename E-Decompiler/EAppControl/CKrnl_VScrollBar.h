#pragma once
#include "EAppControl.h"

//���������

struct CKrnl_VScrollBar :public EAppControl
{
public:
	CKrnl_VScrollBar();
	static CKrnl_VScrollBar* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};