#include "CKrnl_ListBox.h"

CKrnl_ListBox::CKrnl_ListBox()
{
	type = krnl_ListBox;
}


CKrnl_ListBox* CKrnl_ListBox::create()
{
	return new CKrnl_ListBox();
}


std::string CKrnl_ListBox::GetEventName(int eventIndex)
{
	switch (eventIndex)
	{
	case 0:
		return "列表框被选择";
	case 1:
		return "双击选择";
	default:
		break;
	}
	return GetCommonEventName(eventIndex);
}

bool CKrnl_ListBox::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_ListBox::GetPropertyName(unsigned int propertyIndex)
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
		return "自动排序";
	case 10:
		return "多列";
	case 11:
		return "行间距";
	case 12:
		return "文本颜色";
	case 13:
		return "背景颜色";
	case 14:
		return "字体";
	case 15:
		return "允许选择多项";
	case 16:
		return "现行选中项";
	case 17:
		return "列表项目";
	case 18:
		return "项目数值";
	case 19:
		return "数据源";
	case 20:
		return "数据列";
	case 21:
		return "除去重复";
	}
	return "未知属性";
}
