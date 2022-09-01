#include "CKrnl_Button.h"

CKrnl_Button::CKrnl_Button()
{
	type = eSymbol_ControlType::krnl_Button;
}

CKrnl_Button* CKrnl_Button::create()
{
	return new CKrnl_Button();
}

std::string CKrnl_Button::GetEventName(int eventIndex)
{
	switch (eventIndex)
	{
	case 0:
		return "被单击";
	default:
		break;
	}
	return GetCommonEventName(eventIndex);
}

bool CKrnl_Button::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_Button::GetPropertyName(unsigned int propertyIndex)
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
		return "图片";
	case 9:
		return "类型";
	case 10:
		return "标题";
	case 11:
		return "横向对齐方式";
	case 12:
		return "纵向对齐方式";
	case 13:
		return "字体";
	}
	return "未知属性";
}
