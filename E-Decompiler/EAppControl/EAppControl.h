#pragma once
#include <string>
#include <vector>
#include "../ELib.h"

struct EAppControl
{
public:
	//取事件索引对应的名称
	virtual std::string GetEventName(int eventIndex) = 0;
	//加载控件附加属性数据
	virtual bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize);
	//获取属性名称
	virtual std::string GetPropertyName(unsigned int propertyIndex) = 0;
public:
	eSymbol_ControlType type;
	//控件所属窗口ID
	unsigned int windowID;
	//控件自身ID
	unsigned int controlId;
	//控件名称
	std::string controlName;
	//控件所属数据类型的ID
	unsigned int controlTypeId;
	//控件类型名称
	std::string controlTypeName;
	//属性地址
	unsigned int propertyAddr;
	//属性大小
	int propertySize;
	//事件处理
	std::vector<eSymbol_EventInfo> eventList;
};

struct EAppControl_Unknow :public EAppControl
{
public:
	static EAppControl_Unknow* create();
public:
	std::string GetEventName(int eventIndex) override;
	bool InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize) override;
	std::string GetPropertyName(unsigned int propertyIndex) override;
};