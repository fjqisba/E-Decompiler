#include "CKrnl_VScrollBar.h"

CKrnl_VScrollBar::CKrnl_VScrollBar()
{
	type = krnl_VScrollBar;
}

CKrnl_VScrollBar* CKrnl_VScrollBar::create()
{
	return new CKrnl_VScrollBar();
}

std::string CKrnl_VScrollBar::GetEventName(int eventIndex)
{
	switch (eventIndex)
	{
	case 0:
		return "位置被改变";
	default:
		break;
	}
	return GetCommonEventName(eventIndex);
}

bool CKrnl_VScrollBar::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_VScrollBar::GetPropertyName(unsigned int propertyIndex)
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
		return "最小位置";
	case 9:
		return "最大位置";
	case 10:
		return "页改变值";
	case 11:
		return "行改变值";
	case 12:
		return "位置";
	case 13:
		return "允许拖动跟踪";
	}
	return "未知属性";
}
