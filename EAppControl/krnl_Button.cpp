#include "krnl_Button.h"
#include "EAppControl.h"
#include "../common/public.h"

struct Button_UnitData
{
	unsigned int version;
	unsigned int 类型;
	unsigned int 横向对齐方式;
	unsigned int 纵向对齐方式;
	qvector<unsigned char> 图片;
	qvector<unsigned char> 字体;
	qstring 标题;
};

QStringList krnl_Button::取类型列表()
{
	QStringList Items = {
		QStringLiteral("通常"),
		QStringLiteral("默认"),
	};
	return Items;
}

QStringList krnl_Button::取横向对齐方式列表()
{
	QStringList Items = {
		QStringLiteral("左边"),
		QStringLiteral("居中"),
		QStringLiteral("右边"),
	};
	return Items;
}

QStringList krnl_Button::取纵向对齐方式列表()
{
	QStringList Items = {
		QStringLiteral("顶边"),
		QStringLiteral("居中"),
		QStringLiteral("底边"),
	};
	return Items;
}

QString krnl_Button::取图标资源路径()
{
	return QStringLiteral(":/ICO/ICO/按钮.ico");
}

void krnl_Button::反序列化控件附加属性(unsigned char* pUnitDataPtr, QHash<QString, QVariant>& out_data)
{
	Button_UnitData tmpData;

	tmpData.version = CDR_ReadUInt(pUnitDataPtr);
	if (tmpData.version > 1) {
		return;
	}

	tmpData.类型 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.横向对齐方式 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.纵向对齐方式 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.图片 = CDR_ReadCFreqMem(pUnitDataPtr);
	tmpData.字体 = CDR_ReadCFreqMem(pUnitDataPtr);
	tmpData.标题 = CDR_ReadCString(pUnitDataPtr);

	//——————————————————————————
	out_data[QStringLiteral("version")] = tmpData.version;
	out_data[QStringLiteral("类型")] = tmpData.类型;
	out_data[QStringLiteral("横向对齐方式")] = tmpData.横向对齐方式;
	out_data[QStringLiteral("纵向对齐方式")] = tmpData.纵向对齐方式;
	if (tmpData.图片.size()) {
		out_data[QStringLiteral("图片")] = QByteArray((char*)&tmpData.图片[0], tmpData.图片.size());
	}
	if (tmpData.字体.size()) {
		out_data[QStringLiteral("字体")] = QByteArray((char*)&tmpData.字体[0], tmpData.字体.size());
	}
	out_data[QStringLiteral("标题")] = QString::fromLocal8Bit(tmpData.标题.c_str());
}

void krnl_Button::取控件默认附加属性(QHash<QString, QVariant>& out_data)
{
	out_data[QStringLiteral("类型")] = 0;
	out_data[QStringLiteral("横向对齐方式")] = 1;
	out_data[QStringLiteral("横向对齐方式")] = 1;
	out_data[QStringLiteral("标题")] = QStringLiteral("按钮");
}

qstring krnl_Button::取事件名称(int eventIndex)
{
	qstring ret;
	switch (eventIndex)
	{
	case 0:
		ret = "被单击";
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

void krnl_Button::显示控件属性信息(QHash<QString, QVariant>& map_ControlData)
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
	EAppControl::添加布尔控件(QStringLiteral("可停留焦点"), map_ControlData[QStringLiteral("可停留焦点")].toBool());
	EAppControl::添加文本控件(QStringLiteral("    停留顺序"), map_ControlData[QStringLiteral("停留顺序")].toString());
	EAppControl::添加图片控件(QStringLiteral("图片"), map_ControlData[QStringLiteral("图片")].toByteArray());
	EAppControl::添加列表控件(QStringLiteral("类型"), krnl_Button::取类型列表(), map_ControlData[QStringLiteral("类型")].toUInt());
	EAppControl::添加文本控件(QStringLiteral("标题"), map_ControlData[QStringLiteral("标题")].toString());
	EAppControl::添加列表控件(QStringLiteral("横向对齐方式"), krnl_Button::取横向对齐方式列表(), map_ControlData[QStringLiteral("横向对齐方式")].toUInt());
	EAppControl::添加列表控件(QStringLiteral("纵向对齐方式"), krnl_Button::取纵向对齐方式列表(), map_ControlData[QStringLiteral("纵向对齐方式")].toUInt());
	EAppControl::添加字体控件(QStringLiteral("字体"), map_ControlData[QStringLiteral("字体")].toByteArray());
}