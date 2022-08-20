#include "CKrnl_Tab.h"


CKrnl_Tab::CKrnl_Tab()
{
	type = krnl_Tab;
}

CKrnl_Tab* CKrnl_Tab::create()
{
	return new CKrnl_Tab();
}

std::string CKrnl_Tab::GetEventName(int eventIndex)
{
	std::string ret;
	switch (eventIndex)
	{
	case 0:
		ret = "被单击";
		break;
	case 1:
		ret = "将改变子夹";
		break;
	case 2:
		ret = "子夹被改变";
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

bool CKrnl_Tab::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_Tab::GetPropertyName(unsigned int propertyIndex)
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
		return "表头方向";
	case 9:
		return "允许多行表头";
	case 10:
		return "字体";
	case 11:
		return "子夹管理";
	case 12:
		return "现行子夹";
	case 13:
		return "隐藏自身";
	case 14:
		return "是否填充背景";
	case 15:
		return "背景颜色";
	}
	return "未知属性";
}
