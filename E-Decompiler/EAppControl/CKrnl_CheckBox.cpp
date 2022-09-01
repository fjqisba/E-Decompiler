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
	switch (eventIndex)
	{
	case 0:
		return "被单击";
	default:
		break;
	}
	return GetCommonEventName(eventIndex);
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
