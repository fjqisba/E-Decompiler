#include "CKrnl_CheckBox.h"

CKrnl_CheckBox::CKrnl_CheckBox()
{
	type = krnl_CheckBox;
}

CKrnl_CheckBox* CKrnl_CheckBox::create()
{
	return new CKrnl_CheckBox();
}

std::string CKrnl_CheckBox::GetEventName(int eventIndex)
{
	std::string ret;
	switch (eventIndex)
	{
	case 0:
		ret = "被单击";
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
	default:
		ret = "未知事件";
		break;
	}

	return ret;
}

bool CKrnl_CheckBox::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_CheckBox::GetPropertyName(unsigned int propertyIndex)
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
		return "按钮形式";
	case 10:
		return "平面";
	case 11:
		return "标题";
	case 12:
		return "标题居左";
	case 13:
		return "横向对齐方式";
	case 14:
		return "纵向对齐方式";
	case 15:
		return "文本颜色";
	case 16:
		return "背景颜色";
	case 17:
		return "字体";
	case 18:
		return "选中";
	case 19:
		return "数据源";
	case 20:
		return "数据列";
	}
	return "未知属性";
}
