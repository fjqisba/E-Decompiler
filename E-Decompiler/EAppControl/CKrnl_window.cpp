#include "CKrnl_window.h"


CKrnl_window::CKrnl_window()
{
	type = krnl_window;
}

CKrnl_window* CKrnl_window::create()
{
	return new CKrnl_window();
}

std::string CKrnl_window::GetEventName(int eventIndex)
{
	std::string ret;
	switch (eventIndex)
	{
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
	case 0:
		ret = "创建完毕";
		break;
	case 1:
		ret = "可否被关闭";
		break;
	case 2:
		ret = "将被销毁";
		break;
	case 3:
		ret = "位置被改变";
		break;
	case 4:
		ret = "尺寸被改变";
		break;
	case 5:
		ret = "被激活";
		break;
	case 6:
		ret = "被取消激活";
		break;
	case 7:
		ret = "空闲";
		break;
	case 8:
		ret = "首次激活";
		break;
	case 9:
		ret = "托盘事件";
		break;
	case 10:
		ret = "被显示";
		break;
	case 11:
		ret = "被隐藏";
		break;
	case 12:
		ret = "窗口可否被关闭";
		break;
	default:
		ret = "未知事件";
		break;
	}
	return ret;
}

bool CKrnl_window::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{


	return true;
}

std::string CKrnl_window::GetPropertyName(unsigned int propertyIndex)
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
		return "边框";
	case 10:
		return "底色";
	case 11:
		return "底图";
	case 12:
		return "底图方式";
	case 13:
		return "背景音乐";
	case 14:
		return "播放次数";
	case 15:
		return "控制按钮";
	case 16:
		return "最大化按钮";
	case 17:
		return "最小化按钮";
	case 18:
		return "位置";
	case 19:
		return "可否移动";
	case 20:
		return "图标";
	case 21:
		return "回车下移焦点";
	case 22:
		return "Esc键关闭";
	case 23:
		return "F1键打开帮助";
	case 24:
		return "帮助文件名";
	case 25:
		return "帮助标志值";
	case 26:
		return "在任务条中显示";
	case 27:
		return "随意移动";
	case 28:
		return "外形";
	case 29:
		return "总在最前";
	case 30:
		return "保持标题条激活";
	case 31:
		return "窗口类型";
	}
	return "未知属性";
}
