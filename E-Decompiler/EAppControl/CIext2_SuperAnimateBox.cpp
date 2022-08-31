#include "CIext2_SuperAnimateBox.h"

CIext2_SuperAnimateBox::CIext2_SuperAnimateBox()
{
	type = iext2_SuperAnimateBox;
}

CIext2_SuperAnimateBox* CIext2_SuperAnimateBox::create()
{
	return new CIext2_SuperAnimateBox();
}

std::string CIext2_SuperAnimateBox::GetEventName(int eventIndex)
{
	return GetCommonEventName(eventIndex);
}

bool CIext2_SuperAnimateBox::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CIext2_SuperAnimateBox::GetPropertyName(unsigned int propertyIndex)
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
		return "播放位置";
	case 9:
		return "影像文件名";
	case 10:
		return "播放速度";
	case 11:
		return "背景颜色";
	case 12:
		return "透明颜色";
	case 13:
		return "播放";
	case 14:
		return "仅播放一次";
	case 15:
		return "当前帧";
	case 16:
		return "全部帧数";
	}
	return "未知属性";
}

