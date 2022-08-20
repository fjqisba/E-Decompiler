#include "CKrnl_AnimateBox.h"

CKrnl_AnimateBox::CKrnl_AnimateBox()
{
	type = krnl_AnimateBox;
}

CKrnl_AnimateBox* CKrnl_AnimateBox::create()
{
	return new CKrnl_AnimateBox();
}

std::string CKrnl_AnimateBox::GetEventName(int eventIndex)
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
	case -12:
		ret = "滚轮被滚动";
		break;
	default:
		ret = "未知事件";
		break;
	}
	return ret;
}

bool CKrnl_AnimateBox::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_AnimateBox::GetPropertyName(unsigned int propertyIndex)
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
		return "文件名";
	case 9:
		return "居中播放";
	case 10:
		return "透明背景";
	case 11:
		return "播放";
	case 12:
		return "播放次数";
	}
	return "未知属性";
}
