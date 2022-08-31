#include "CIext2_RichEdit.h"

CIext2_RichEdit::CIext2_RichEdit()
{
	type = iext2_RichEdit;
}

CIext2_RichEdit* CIext2_RichEdit::create()
{
	return new CIext2_RichEdit();
}

std::string CIext2_RichEdit::GetEventName(int eventIndex)
{
	switch (eventIndex)
	{
	case 0:
		return "内容被改变";
	case 1:
		return "选择区被改变";
	default:
		break;
	}
	return GetCommonEventName(eventIndex);
}

bool CIext2_RichEdit::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CIext2_RichEdit::GetPropertyName(unsigned int propertyIndex)
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
		return "内容格式";
	case 9:
		return "内容";
	case 10:
		return "边框";
	case 11:
		return "背景颜色";
	case 12:
		return "是否允许多行";
	case 13:
		return "自动换行";
	case 14:
		return "只读";
	case 15:
		return "隐藏选择";
	case 16:
		return "最大允许长度";
	case 17:
		return "起始选择位置";
	case 18:
		return "被选择字符数";
	case 19:
		return "被选择文本";
	case 20:
		return "是否已更改";
	}
	return "未知属性";
}

