#include "CKrnl_HScrollBar.h"


CKrnl_HScrollBar::CKrnl_HScrollBar()
{
	type = krnl_HScrollBar;
}

CKrnl_HScrollBar* CKrnl_HScrollBar::create()
{
	return new CKrnl_HScrollBar();
}

std::string CKrnl_HScrollBar::GetEventName(int eventIndex)
{
	std::string ret;
	switch (eventIndex)
	{
	case 0:
		ret = "位置被改变";
		break;
	case -1:
		ret = "鼠标左键被按下";
		break;
	case -2:
		ret = "鼠标左键被放开";
		break;
	case -3:
		ret = "被双击";
		break;
	case -4:
		ret = "鼠标右键被按下";
		break;
	case -5:
		ret = "鼠标右键被放开";
		break;
	case -6:
		ret = "鼠标位置被移动";
		break;
	case -7:
		ret = "获得焦点";
		break;
	case -8:
		ret = "失去焦点";
		break;
	case -9:
		ret = "按下某键";
		break;
	case -10:
		ret = "放开某键";
		break;
	case -11:
		ret = "字符输入";
		break;
	case -12:
		ret = "滚轮被滚动";
		break;
	default:
		ret = "未知事件";
		break;
	}

	return ret;
}

bool CKrnl_HScrollBar::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_HScrollBar::GetPropertyName(unsigned int propertyIndex)
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
