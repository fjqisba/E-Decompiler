#pragma once
#include "EAppControl.h"

//´°¿Ú

struct CKrnl_window :public EAppControl
{
public:
	CKrnl_window();
	static CKrnl_window* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};