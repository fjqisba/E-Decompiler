#include "krnl_EditBox.h"

void krnl_EditBox::取控件默认附加属性(QHash<QString, QVariant>& out_data)
{

}

void krnl_EditBox::反序列化控件附加属性(unsigned char* pUnitDataPtr, QHash<QString, QVariant>& out_data)
{

}

void krnl_EditBox::显示控件属性信息(QHash<QString, QVariant>& out_data)
{

}

qstring krnl_EditBox::取事件名称(int eventIndex)
{
	qstring ret;
	switch (eventIndex)
	{
	case 0:
		ret = "内容被改变";
		break;
	case 1:
		ret = "调节钮被按下";
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
		break;
	default:
		ret = "未知事件";
		break;
	}

	return ret;
}