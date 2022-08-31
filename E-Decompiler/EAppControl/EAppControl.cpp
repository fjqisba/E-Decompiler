#include "EAppControl.h"

bool EAppControl::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string EAppControl::GetCommonEventName(int eventIndex)
{
	switch (eventIndex)
	{
	case -1:
		return "鼠标左键被按下";
	case -2:
		return "鼠标左键被放开";
	case -3:
		return "被双击";
	case -4:
		return "鼠标右键被按下";
	case -5:
		return "鼠标右键被放开";
	case -6:
		return "鼠标位置被移动";
	case -7:
		return "获得焦点";
	case -8:
		return "失去焦点";
	case -9:
		return "按下某键";
	case -10:
		return "放开某键";
	case -11:
		return "字符输入";
	case -12:
		return "滚轮被滚动";
	}
	return "未知事件";

}
