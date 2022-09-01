#include "CKrnl_EditBox.h"

CKrnl_EditBox::CKrnl_EditBox()
{
	type = krnl_EditBox;
}

CKrnl_EditBox* CKrnl_EditBox::create()
{
	return new CKrnl_EditBox();
}

std::string CKrnl_EditBox::GetEventName(int eventIndex)
{
	switch (eventIndex)
	{
	case 0:
		return "内容被改变";
	case 1:
		return "调节钮被按下";
	default:
		break;
	}
	return GetCommonEventName(eventIndex);
}

bool CKrnl_EditBox::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_EditBox::GetPropertyName(unsigned int propertyIndex)
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
		return "内容";
	case 9:
		return "边框";
	case 10:
		return "文本颜色";
	case 11:
		return "背景颜色";
	case 12:
		return "字体";
	case 13:
		return "隐藏选择";
	case 14:
		return "最大允许长度";
	case 15:
		return "是否允许多行";
	case 16:
		return "滚动条";
	case 17:
		return "对齐方式";
	case 18:
		return "输入方式";
	case 19:
		return "密码遮盖字符";
	case 20:
		return "转换方式";
	case 21:
		return "调节器方式";
	case 22:
		return "调节器底限值";
	case 23:
		return "调节器上限值";
	case 24:
		return "起始选择位置";
	case 25:
		return "被选择字符数";
	case 26:
		return "被选择文本";
	case 27:
		return "数据源";
	case 28:
		return "数据列";
	}
	return "未知属性";
}
