#include "CKrnl_ComboBox.h"

CKrnl_ComboBox::CKrnl_ComboBox()
{
	type = krnl_ComboBox;
}

CKrnl_ComboBox* CKrnl_ComboBox::create()
{
	return new CKrnl_ComboBox();
}

std::string CKrnl_ComboBox::GetEventName(int eventIndex)
{
	switch (eventIndex)
	{
	case 0:
		return "列表项被选择";
	case 1:
		return "编辑内容被改变";
	case 2:
		return "将弹出列表";
	case 3:
		return "列表被关闭";
	case 4:
		return "双击选择";
	default:
		break;
	}
	return GetCommonEventName(eventIndex);
}

bool CKrnl_ComboBox::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_ComboBox::GetPropertyName(unsigned int propertyIndex)
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
		return "内容";
	case 10:
		return "最大文本长度";
	case 11:
		return "起始选择位置";
	case 12:
		return "被选择字符数";
	case 13:
		return "被选择文本";
	case 14:
		return "自动排序";
	case 15:
		return "行间距";
	case 16:
		return "文本颜色";
	case 17:
		return "背景颜色";
	case 18:
		return "字体";
	case 19:
		return "现行选中项";
	case 20:
		return "列表项目";
	case 21:
		return "项目数值";
	case 22:
		return "数据源";
	case 23:
		return "数据列";
	case 24:
		return "除去重复";
	}
	return "未知属性";
}
