#include "CIext2_CartoonBox.h"

CIext2_CartoonBox::CIext2_CartoonBox()
{
	type = iext2_CartoonBox;
}

CIext2_CartoonBox* CIext2_CartoonBox::create()
{
	return new CIext2_CartoonBox();
}

std::string CIext2_CartoonBox::GetEventName(int eventIndex)
{
	switch (eventIndex)
	{
	case 0:
		return "物体左键被按下";
	case 1:
		return "物体右键被按下";
	case 2:
		return "物体左键被放开";
	case 3:
		return "物体右键被放开";
	case 4:
		return "双击物体";
	case 5:
		return "进入物体";
	case 6:
		return "离开物体";
	case 7:
		return "动画框鼠标位置改变";
	case 8:
		return "物体位置将改变";
	case 9:
		return "物体位置已改变";
	case 10:
		return "物体将销毁";
	case 11:
		return "碰撞到物体";
	case 12:
		return "碰撞到边界";
	case 13:
		return "越出边界";
	case 14:
		return "自动前进停止";
	case 15:
		return "自动旋转停止";
	case 16:
		return "动画播放完毕";
	case 17:
		return "监视键被按下";
	default:
		break;
	}
	return GetCommonEventName(eventIndex);
}

bool CIext2_CartoonBox::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string CIext2_CartoonBox::GetPropertyName(unsigned int propertyIndex)
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
		return "背景颜色";
	case 10:
		return "背景图片";
	case 11:
		return "显示方式";
	case 12:
		return "画笔类型";
	case 13:
		return "画出方式";
	case 14:
		return "画笔粗细";
	case 15:
		return "画笔颜色";
	case 16:
		return "刷子类型";
	case 17:
		return "刷子颜色";
	case 18:
		return "文本颜色";
	case 19:
		return "文本背景颜色";
	case 20:
		return "字体";
	}
	return "未知属性";
}

