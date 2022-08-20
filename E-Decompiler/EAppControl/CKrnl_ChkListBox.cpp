#include "CKrnl_ChkListBox.h"


CKrnl_ChkListBox::CKrnl_ChkListBox()
{
	type = krnl_ChkListBox;
}

CKrnl_ChkListBox* CKrnl_ChkListBox::create()
{
	return new CKrnl_ChkListBox();
}

std::string CKrnl_ChkListBox::GetEventName(int eventIndex)
{
	std::string ret;
	switch (eventIndex)
	{
	case 0:
		ret = "列表项被选择";
		break;
	case 1:
		ret = "选中状态被改变";
		break;
	case 2:
		ret = "双击选择";
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
	case -7:
		ret = "获得焦点";
		break;
	case -8:
		ret = "失去焦点";
		break;
	case -9:
		ret = "按下某键";
		break;
	case -10:
		ret = "放开某键";
		break;
	case -11:
		ret = "字符输入";
		break;
	case -12:
		ret = "滚轮被滚动";
	default:
		ret = "未知事件";
		break;
	}

	return ret;
}

bool CKrnl_ChkListBox::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_ChkListBox::GetPropertyName(unsigned int propertyIndex)
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
		return "单选";
	case 13:
		return "字体";
	case 14:
		return "现行选中项";
	case 15:
		return "列表项目";
	case 16:
		return "项目数值";
	}
	return "未知属性";
}
