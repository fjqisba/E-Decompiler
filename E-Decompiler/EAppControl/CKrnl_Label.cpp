#include "CKrnl_Label.h"


CKrnl_Label::CKrnl_Label()
{
	type = krnl_Label;
}

CKrnl_Label* CKrnl_Label::create()
{
	return new CKrnl_Label();
}

std::string CKrnl_Label::GetEventName(int eventIndex)
{
	std::string ret;
	switch (eventIndex)
	{
	case 0:
		ret = "反馈事件";
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
	case -12:
		ret = "滚轮被滚动";
		break;
	default:
		ret = "未知事件";
		break;
	}
	return ret;
}

bool CKrnl_Label::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_Label::GetPropertyName(unsigned int propertyIndex)
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
		return "标题";
	case 9:
		return "效果";
	case 10:
		return "边框";
	case 11:
		return "渐变边框宽度";
	case 12:
		return "渐变边框颜色1";
	case 13:
		return "渐变边框颜色2";
	case 14:
		return "渐变边框颜色3";
	case 15:
		return "字体";
	case 16:
		return "文本颜色";
	case 17:
		return "背景颜色";
	case 18:
		return "底图";
	case 19:
		return "底图方式";
	case 20:
		return "渐变背景方式";
	case 21:
		return "渐变背景颜色1";
	case 22:
		return "渐变背景颜色2";
	case 23:
		return "渐变背景颜色3";
	case 24:
		return "横向对齐方式";
	case 25:
		return "是否自动折行";
	case 26:
		return "纵向对齐方式";
	case 27:
		return "数据源";
	case 28:
		return "数据列";
	}
	return "未知属性";
}
