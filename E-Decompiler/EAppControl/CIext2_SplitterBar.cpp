#include "CIext2_SplitterBar.h"

CIext2_SplitterBar::CIext2_SplitterBar()
{
	type = iext2_SplitterBar;
}

CIext2_SplitterBar* CIext2_SplitterBar::create()
{
	return new CIext2_SplitterBar();
}

std::string CIext2_SplitterBar::GetEventName(int eventIndex)
{
	switch (eventIndex)
	{
	case 0:
		return "被拖动";
	default:
		break;
	}
	return GetCommonEventName(eventIndex);
}

bool CIext2_SplitterBar::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CIext2_SplitterBar::GetPropertyName(unsigned int propertyIndex)
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
		return "背景颜色";
	case 9:
		return "方向";
	}
	return "未知属性";
}

