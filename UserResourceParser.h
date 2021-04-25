#pragma once
#include <pro.h>
#include <QVariant>

//常量资源解析引擎
class UserResourceParser
{
public:
	enum BinType_t
	{
		e_UnknownValue = 0,
		e_NullBin,
		e_BinValue,
		e_NullStr,
		e_StringValue,
		e_ArrayHead,
		e_FloatValue,
		e_ClassTable,
		e_SwitchTable,         //switch case表
	};
	struct mid_BinSource
	{
		BinType_t itype;
		ea_t address;
		QVariant data;
	};
public:
	static void InitUserCodeAddr(ea_t start, ea_t end);
	static bool ParseUserResource(ea_t lpStringStart, uint32 StringSize);
	static int MenuHandle_ShowUserResource();
private:
	static BinType_t GetBinValueType(ea_t DataAddr);
public:
	static ea_t m_UserCodeStartAddr;
	static ea_t m_UserCodeEndAddr;
};