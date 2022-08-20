#include "CKrnl_PicBtn.h"

CKrnl_PicBtn::CKrnl_PicBtn()
{
	type = krnl_PicBtn;
}

CKrnl_PicBtn* CKrnl_PicBtn::create()
{
	return new CKrnl_PicBtn();
}

std::string CKrnl_PicBtn::GetEventName(int eventIndex)
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
	case -12:
		ret = "滚轮被滚动";
		break;
	default:
		ret = "未知事件";
		break;
	}
	return ret;
}

bool CKrnl_PicBtn::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_PicBtn::GetPropertyName(unsigned int propertyIndex)
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
		return "类型";
	case 9:
		return "选中";
	case 10:
		return "正常图片";
	case 11:
		return "点燃图片";
	case 12:
		return "按下图片";
	case 13:
		return "禁止图片";
	case 14:
		return "透明颜色";
	}
	return "未知属性";
}
