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
	case -1:
		return "鼠标左键被按下";
	case -2:
		return "鼠标左键被放开";
	case -3:
		return "被双击";
		break;
	case -4:
		return "鼠标右键被按下";
		break;
	case -5:
		return "鼠标右键被放开";
		break;
	case -6:
		return "鼠标位置被移动";
		break;
	case -7:
		return "获得焦点";
		break;
	case -8:
		return "失去焦点";
		break;
	case -9:
		return "按下某键";
		break;
	case -10:
		return "放开某键";
		break;
	case -11:
		return "字符输入";
		break;
	case -12:
		return "滚轮被滚动";
		break;
	}
	return "未知事件";
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
