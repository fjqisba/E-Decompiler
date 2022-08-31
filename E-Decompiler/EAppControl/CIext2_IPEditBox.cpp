#include "CIext2_IPEditBox.h"

CIext2_IPEditBox::CIext2_IPEditBox()
{
	type = iext2_IPEditBox;
}

CIext2_IPEditBox* CIext2_IPEditBox::create()
{
	return new CIext2_IPEditBox();
}

std::string CIext2_IPEditBox::GetEventName(int eventIndex)
{
	switch (eventIndex)
	{
	case 0:
		return "地址被改变";
	default:
		break;
	}
	return GetCommonEventName(eventIndex);
}

bool CIext2_IPEditBox::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CIext2_IPEditBox::GetPropertyName(unsigned int propertyIndex)
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
		return "字体";
	case 9:
		return "地址";
	}
	return "未知属性";
}

