#include "CKrnl_SliderBar.h"


CKrnl_SliderBar::CKrnl_SliderBar()
{
	type = krnl_SliderBar;
}

CKrnl_SliderBar* CKrnl_SliderBar::create()
{
	return new CKrnl_SliderBar();
}

std::string CKrnl_SliderBar::GetEventName(int eventIndex)
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

bool CKrnl_SliderBar::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_SliderBar::GetPropertyName(unsigned int propertyIndex)
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
		return "方向";
	case 10:
		return "刻度类型";
	case 11:
		return "单位刻度值";
	case 12:
		return "允许选择";
	case 13:
		return "首选择位置";
	case 14:
		return "选择长度";
	case 15:
		return "页改变值";
	case 16:
		return "行改变值";
	case 17:
		return "最小位置";
	case 18:
		return "最大位置";
	case 19:
		return "位置";
	}
	return "未知属性";
}
