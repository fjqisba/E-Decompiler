#include "krnl_Timer.h"
#include "../common/public.h"

struct Timer_UnitData
{
	unsigned int version;
	unsigned int 时钟周期;
};

QString krnl_Timer::取图标资源路径()
{
	return QStringLiteral(":/ICO/ICO/时钟.ico");
}

void krnl_Timer::取控件默认附加属性(QHash<QString, QVariant>& out_data)
{
	out_data[QStringLiteral("时钟周期")] = 0;
}

void krnl_Timer::反序列化控件附加属性(unsigned char* pUnitDataPtr, QHash<QString, QVariant>& out_data)
{
	Timer_UnitData tmpData;

	tmpData.version = CDR_ReadUInt(pUnitDataPtr);
	if (tmpData.version > 1) {
		return;
	}
	tmpData.时钟周期 = CDR_ReadUInt(pUnitDataPtr);

	out_data[QStringLiteral("version")] = tmpData.version;
	out_data[QStringLiteral("时钟周期")] = tmpData.时钟周期;
	return;
}

void krnl_Timer::显示控件属性信息(QHash<QString, QVariant>& map_ControlData)
{
	EAppControl::添加文本控件(QStringLiteral("名称"), map_ControlData[QStringLiteral("名称")].toString());
	EAppControl::添加文本控件(QStringLiteral("左边"), map_ControlData[QStringLiteral("左边")].toString());
	EAppControl::添加文本控件(QStringLiteral("顶边"), map_ControlData[QStringLiteral("顶边")].toString());
	EAppControl::添加文本控件(QStringLiteral("宽度"), map_ControlData[QStringLiteral("宽度")].toString());
	EAppControl::添加文本控件(QStringLiteral("高度"), map_ControlData[QStringLiteral("高度")].toString());
	EAppControl::添加文本控件(QStringLiteral("标记"), map_ControlData[QStringLiteral("标记")].toString());
	EAppControl::添加文本控件(QStringLiteral("时钟周期"), map_ControlData[QStringLiteral("时钟周期")].toString());
	return;
}

qstring krnl_Timer::取事件名称(int eventIndex)
{
	qstring ret;
	switch (eventIndex)
	{
	case 0:
		ret = "周期事件";
		break;
	default:
		ret = "未知事件";
		break;
	}
	return ret;
}