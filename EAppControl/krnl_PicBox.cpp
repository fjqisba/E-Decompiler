#include "krnl_PicBox.h"
#include "../common/public.h"

struct PicBox_UnitData
{
	unsigned int version;
	unsigned int 边框;
	unsigned int 背景颜色;
	unsigned int 显示方式;
	unsigned int 播放动画;
	qvector<unsigned char> 图片;
	qstring 数据源;
	qstring 数据列;
};

QStringList krnl_PicBox::取边框列表()
{
	QStringList Items = {
	QStringLiteral("无边框"),
	QStringLiteral("凹入式"),
	QStringLiteral("凸出式"),
	QStringLiteral("浅凹入式"),
	QStringLiteral("镜框式"),
	QStringLiteral("单线边框式"),
	};
	return Items;
}

QStringList krnl_PicBox::取显示方式列表()
{
	QStringList Items = {
	QStringLiteral("图片居左上"),
	QStringLiteral("缩放图片"),
	QStringLiteral("图片居中"),
	};
	return Items;
}

void krnl_PicBox::取控件默认附加属性(QHash<QString, QVariant>& out_data)
{
	out_data[QStringLiteral("边框")] = 0;
	out_data[QStringLiteral("背景颜色")] = 0xFF000000;
	out_data[QStringLiteral("显示方式")] = 0;
	out_data[QStringLiteral("播放动画")] = 1;
}

void krnl_PicBox::反序列化控件附加属性(unsigned char* pUnitDataPtr, QHash<QString, QVariant>& out_data)
{
	PicBox_UnitData tmpData;
	tmpData.version = CDR_ReadUInt(pUnitDataPtr);
	if (tmpData.version > 2) {
		return;
	}
	tmpData.边框 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.背景颜色 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.显示方式 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.播放动画 = CDR_ReadUInt(pUnitDataPtr);

	tmpData.图片 = CDR_ReadCFreqMem(pUnitDataPtr);
	if (tmpData.version == 2) {
		tmpData.数据源 = CDR_ReadCString(pUnitDataPtr);
		tmpData.数据列 = CDR_ReadCString(pUnitDataPtr);
	}

	//——————————————————————
	out_data[QStringLiteral("version")] = tmpData.version;
	out_data[QStringLiteral("边框")] = tmpData.边框;
	out_data[QStringLiteral("背景颜色")] = tmpData.背景颜色;
	out_data[QStringLiteral("显示方式")] = tmpData.显示方式;
	out_data[QStringLiteral("播放动画")] = tmpData.播放动画;
	if (tmpData.图片.size()) {
		out_data[QStringLiteral("图片")] = QByteArray((char*)&tmpData.图片[0], tmpData.图片.size());
	}
	out_data[QStringLiteral("数据源")] = QString::fromLocal8Bit(tmpData.数据源.c_str());
	out_data[QStringLiteral("数据列")] = QString::fromLocal8Bit(tmpData.数据列.c_str());
	return;
}

void krnl_PicBox::显示控件属性信息(QHash<QString, QVariant>& map_ControlData)
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
	EAppControl::添加列表控件(QStringLiteral("边框"), krnl_PicBox::取边框列表(), map_ControlData[QStringLiteral("边框")].toUInt());
	EAppControl::添加颜色控件(QStringLiteral("背景颜色"), map_ControlData[QStringLiteral("背景颜色")].toUInt());
	EAppControl::添加图片控件(QStringLiteral("图片"), map_ControlData[QStringLiteral("图片")].toByteArray());
	EAppControl::添加列表控件(QStringLiteral("    显示方式"), krnl_PicBox::取显示方式列表(), map_ControlData[QStringLiteral("显示方式")].toUInt());
	EAppControl::添加布尔控件(QStringLiteral("播放动画"), map_ControlData[QStringLiteral("播放动画")].toBool());
	EAppControl::添加文本控件(QStringLiteral("数据源"), map_ControlData[QStringLiteral("数据源")].toString());
	EAppControl::添加文本控件(QStringLiteral("数据列"), map_ControlData[QStringLiteral("数据列")].toString());
	return;
}

QString krnl_PicBox::取图标资源路径()
{
	return QStringLiteral(":/ICO/ICO/图片框.ico");
}

qstring krnl_PicBox::取事件名称(int eventIndex)
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