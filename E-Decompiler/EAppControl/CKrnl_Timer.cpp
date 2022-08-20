#include "CKrnl_Timer.h"


CKrnl_Timer::CKrnl_Timer()
{
	type = krnl_Timer;
}

CKrnl_Timer* CKrnl_Timer::create()
{
	return new CKrnl_Timer();
}

std::string CKrnl_Timer::GetEventName(int eventIndex)
{
	std::string ret;
	switch (eventIndex)
	{
	case 0:
		ret = "周期事件";
		break;
	default:
		ret = "未知事件";
		break;
	}
	return ret;
}

bool CKrnl_Timer::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_Timer::GetPropertyName(unsigned int propertyIndex)
{
	switch (propertyIndex)
	{
	case 0:
		return "左边";
	case 1:
		return "顶边";
	case 2:
		return "宽度";
	case 3:
		return "高度";
	case 4:
		return "标记";
	case 5:
		return "可视";
	case 6:
		return "禁止";
	case 7:
		return "鼠标指针";
	case 8:
		return "时钟周期";
	}
	return "未知属性";
}
