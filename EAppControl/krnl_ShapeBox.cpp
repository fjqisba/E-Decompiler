#include "krnl_ShapeBox.h"
#include "../common/public.h"

struct ShapeBox_UnitData
{
	unsigned int version;
	unsigned int 背景颜色;
	unsigned int 外形;
	unsigned int 线条效果;
	unsigned int 线型;
	unsigned int 线宽;
	unsigned int 线条颜色;
	unsigned int 填充颜色;
};

QStringList krnl_ShapeBox::取外形列表()
{
	QStringList Items = {
		QStringLiteral("矩形"),
		QStringLiteral("正方形"),
		QStringLiteral("椭圆"),
		QStringLiteral("圆"),
		QStringLiteral("圆角矩形"),
		QStringLiteral("圆角正方形"),
		QStringLiteral("横向线"),
		QStringLiteral("纵向线"),
	};
	return Items;
}

QStringList krnl_ShapeBox::取线条效果列表()
{
	QStringList Items = {
	QStringLiteral("通常"),
	QStringLiteral("凹入"),
	QStringLiteral("凸出"),
	};
	return Items;
}

QStringList krnl_ShapeBox::取线型列表()
{
	QStringList Items = {
	QStringLiteral("无"),
	QStringLiteral("直线"),
	QStringLiteral("划线"),
	QStringLiteral("点线"),
	QStringLiteral("点划线"),
	QStringLiteral("双点划线"),
	};
	return Items;
}

void krnl_ShapeBox::取控件默认附加属性(QHash<QString, QVariant>& out_data)
{
	out_data[QStringLiteral("背景颜色")] = 0xFF000000;
	out_data[QStringLiteral("外形")] = 0x0;
	out_data[QStringLiteral("线条效果")] = 0x0;
	out_data[QStringLiteral("线型")] = 0x1;
	out_data[QStringLiteral("线宽")] = 0x1;
	out_data[QStringLiteral("线条颜色")] = 0x0;
	out_data[QStringLiteral("填充颜色")] = 0xFF000000;
}

void krnl_ShapeBox::反序列化控件附加属性(unsigned char* pUnitDataPtr, QHash<QString, QVariant>& out_data)
{
	ShapeBox_UnitData tmpData;

	tmpData.version = CDR_ReadUInt(pUnitDataPtr);
	if (tmpData.version > 1) {
		return;
	}

	tmpData.背景颜色 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.外形 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.线条效果 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.线型 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.线宽 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.线条颜色 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.填充颜色 = CDR_ReadUInt(pUnitDataPtr);

	out_data[QStringLiteral("背景颜色")] = tmpData.背景颜色;
	out_data[QStringLiteral("外形")] = tmpData.外形;
	out_data[QStringLiteral("线条效果")] = tmpData.线条效果;
	out_data[QStringLiteral("线型")] = tmpData.线型;
	out_data[QStringLiteral("线宽")] = tmpData.线宽;
	out_data[QStringLiteral("线条颜色")] = tmpData.线条颜色;
	out_data[QStringLiteral("填充颜色")] = tmpData.填充颜色;
	return;
}

void krnl_ShapeBox::显示控件属性信息(QHash<QString, QVariant>& map_ControlData)
{
	EAppControl::添加文本控件(QStringLiteral("名称"), map_ControlData[QStringLiteral("名称")].toString());
	EAppControl::添加文本控件(QStringLiteral("左边"), map_ControlData[QStringLiteral("左边")].toString());
	EAppControl::添加文本控件(QStringLiteral("顶边"), map_ControlData[QStringLiteral("顶边")].toString());
	EAppControl::添加文本控件(QStringLiteral("宽度"), map_ControlData[QStringLiteral("宽度")].toString());
	EAppControl::添加文本控件(QStringLiteral("高度"), map_ControlData[QStringLiteral("高度")].toString());
	EAppControl::添加文本控件(QStringLiteral("标记"), map_ControlData[QStringLiteral("标记")].toString());
	EAppControl::添加布尔控件(QStringLiteral("可视"), map_ControlData[QStringLiteral("可视")].toBool());
	EAppControl::添加布尔控件(QStringLiteral("禁止"), map_ControlData[QStringLiteral("禁止")].toBool());
	EAppControl::添加鼠标控件(map_ControlData[QStringLiteral("鼠标指针")].toUInt());

	EAppControl::添加颜色控件(QStringLiteral("背景颜色"), map_ControlData[QStringLiteral("背景颜色")].toUInt());
	EAppControl::添加列表控件(QStringLiteral("外形"), krnl_ShapeBox::取外形列表(), map_ControlData[QStringLiteral("外形")].toUInt());
	EAppControl::添加列表控件(QStringLiteral("线条效果"), krnl_ShapeBox::取线条效果列表(), map_ControlData[QStringLiteral("线条效果")].toUInt());
	EAppControl::添加列表控件(QStringLiteral("线型"), krnl_ShapeBox::取线型列表(), map_ControlData[QStringLiteral("线型")].toUInt());
	EAppControl::添加文本控件(QStringLiteral("线宽"), map_ControlData[QStringLiteral("线宽")].toString());
	EAppControl::添加颜色控件(QStringLiteral("线条颜色"), map_ControlData[QStringLiteral("线条颜色")].toUInt());
	EAppControl::添加颜色控件(QStringLiteral("填充颜色"), map_ControlData[QStringLiteral("填充颜色")].toUInt());
}

QString krnl_ShapeBox::取图标资源路径()
{
	return QStringLiteral(":/ICO/ICO/外形框.ico");
}

qstring krnl_ShapeBox::取事件名称(int eventIndex)
{
	qstring ret;
	switch (eventIndex)
	{
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
	case -12:
		ret = "滚轮被滚动";
		break;
	default:
		ret = "未知事件";
		break;
	}

	return ret;
}