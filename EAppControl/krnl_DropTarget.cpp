#include "krnl_DropTarget.h"

qstring krnl_DropTarget::取事件名称(int eventIndex)
{
	qstring ret;
	switch (eventIndex)
	{
	case 0:
		ret = "得到文本";
		break;
	case 1:
		ret = "得到超文本";
		break;
	case 2:
		ret = "得到URL";
		break;
	case 3:
		ret = "得到文件";
		break;
	default:
		ret = "未知事件";
		break;
	}

	return ret;
}