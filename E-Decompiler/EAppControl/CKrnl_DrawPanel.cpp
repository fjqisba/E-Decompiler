#include "CKrnl_DrawPanel.h"

CKrnl_DrawPanel::CKrnl_DrawPanel()
{
	type = krnl_DrawPanel;
}

CKrnl_DrawPanel* CKrnl_DrawPanel::create()
{
	return new CKrnl_DrawPanel();
}

std::string CKrnl_DrawPanel::GetEventName(int eventIndex)
{
	std::string ret;
	switch (eventIndex)
	{
	case 0:
		ret = "绘画";
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

bool CKrnl_DrawPanel::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_DrawPanel::GetPropertyName(unsigned int propertyIndex)
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
		return "边框";
	case 9:
		return "画板背景色";
	case 10:
		return "底图";
	case 11:
		return "底图方式";
	case 12:
		return "自动重画";
	case 13:
		return "绘画单位";
	case 14:
		return "画笔类型";
	case 15:
		return "画出方式";
	case 16:
		return "画笔粗细";
	case 17:
		return "画笔颜色";
	case 18:
		return "刷子类型";
	case 19:
		return "刷子颜色";
	case 20:
		return "文本颜色";
	case 21:
		return "文本背景颜色";
	case 22:
		return "字体";
	case 23:
		return "画板宽度";
	case 24:
		return "画板高度";
	}
	return "未知属性";
}
