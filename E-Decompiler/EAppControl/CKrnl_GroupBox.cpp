#include "CKrnl_GroupBox.h"

CKrnl_GroupBox::CKrnl_GroupBox()
{
	type = krnl_GroupBox;
}

CKrnl_GroupBox* CKrnl_GroupBox::create()
{
	return new CKrnl_GroupBox();
}

std::string CKrnl_GroupBox::GetEventName(int eventIndex)
{
	return GetCommonEventName(eventIndex);
}

bool CKrnl_GroupBox::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_GroupBox::GetPropertyName(unsigned int propertyIndex)
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
		return "对齐方式";
	case 10:
		return "文本颜色";
	case 11:
		return "背景颜色";
	case 12:
		return "字体";
	}
	return "未知属性";
}
