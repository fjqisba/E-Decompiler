#pragma once
#include "EAppControl.h"

//Í¼Æ¬¿ò


struct CKrnl_PicBox :public EAppControl
{
public:
	CKrnl_PicBox();
	static CKrnl_PicBox* create();
public:
	std::string GetEventName(int eventIndex)override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};