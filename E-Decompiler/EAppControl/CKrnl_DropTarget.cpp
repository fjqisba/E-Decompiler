#include "CKrnl_DropTarget.h"


CKrnl_DropTarget::CKrnl_DropTarget()
{
	type = krnl_DropTarget;
}

CKrnl_DropTarget* CKrnl_DropTarget::create()
{
	return new CKrnl_DropTarget();
}

std::string CKrnl_DropTarget::GetEventName(int eventIndex)
{
	std::string ret;
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

bool CKrnl_DropTarget::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CKrnl_DropTarget::GetPropertyName(unsigned int propertyIndex)
{
	return "未知属性";
}
