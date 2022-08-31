#include "CIext2_SuperBtn.h"

CIext2_SuperBtn::CIext2_SuperBtn()
{
	type = iext2_SuperBtn;
}

CIext2_SuperBtn* CIext2_SuperBtn::create()
{
	return new CIext2_SuperBtn();
}

std::string CIext2_SuperBtn::GetEventName(int eventIndex)
{
	switch (eventIndex)
	{
	case 0:
		return "被单击";
	default:
		break;
	}
	return GetCommonEventName(eventIndex);
}

bool CIext2_SuperBtn::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CIext2_SuperBtn::GetPropertyName(unsigned int propertyIndex)
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
		return "按钮风格";
	case 9:
		return "选择框方式";
	case 10:
		return "选中";
	case 11:
		return "对齐方式";
	case 12:
		return "压入方式";
	case 13:
		return "绘制边框";
	case 14:
		return "绘制焦点";
	case 15:
		return "通常图片";
	case 16:
		return "通常图片透明色";
	case 17:
		return "点燃图片";
	case 18:
		return "点燃图片透明色";
	case 19:
		return "禁止图片";
	case 20:
		return "禁止图片透明色";
	case 21:
		return "字体";
	case 22:
		return "标题";
	case 23:
		return "使用默认色";
	case 24:
		return "通常前景色";
	case 25:
		return "通常背景色";
	case 26:
		return "点燃前景色";
	case 27:
		return "点燃背景色";
	case 28:
		return "焦点前景色";
	case 29:
		return "焦点背景色";
	case 30:
		return "标准音提示";
	case 31:
		return "经过提示音";
	case 32:
		return "单击提示音";
	case 33:
		return "提示文本";
	case 34:
		return "默认类型";
	}
	return "未知事件";
}

