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
	switch (eventIndex)
	{
	case 0:
		return "创建完毕";
	case 1:
		return "可否被关闭";
	case 2:
		return "将被销毁";
	case 3:
		return "位置被改变";
	case 4:
		return "尺寸被改变";
	case 5:
		return "被激活";
	case 6:
		return "被取消激活";
	case 7:
		return "空闲";
	case 8:
		return "首次激活";
	case 9:
		return "托盘事件";
	case 10:
		return "被显示";
	case 11:
		return "被隐藏";
	case 12:
		return "窗口可否被关闭";
	default:
		break;
	}
	return GetCommonEventName(eventIndex);
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
