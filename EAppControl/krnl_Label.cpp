#include "krnl_Label.h"
#include "../GuiParser.h"
#include "../ControlInfoWidget.h"
#include "../PropertyDelegate.h"
#include "../common/public.h"

struct Label_UnitData
{
	unsigned int version;
	unsigned int 效果;
	unsigned int 渐变边框宽度;
	unsigned int 边框;
	unsigned int 渐变边框颜色1;
	unsigned int 渐变边框颜色2;
	unsigned int 渐变边框颜色3;
	unsigned int 文本颜色;
	unsigned int 背景颜色;
	unsigned int 底图方式;
	unsigned int 渐变背景方式;
	unsigned int 渐变背景颜色1;
	unsigned int 渐变背景颜色2;
	unsigned int 渐变背景颜色3;
	unsigned int 横向对齐方式;
	unsigned int 纵向对齐方式;
	unsigned int 是否自动折行;
	qvector<unsigned char> 字体;
	qvector<unsigned char> 底图;
	qstring 标题;
	qstring 数据源;
	qstring 数据列;
};

QStringList krnl_Label::取效果列表()
{
	QStringList Items = {
	QStringLiteral("通常"),
	QStringLiteral("凹入"),
	QStringLiteral("凸出"),
	QStringLiteral("阴影"),
	};
	return Items;
}

QStringList krnl_Label::取底图方式列表()
{
	QStringList Items = {
	QStringLiteral("图片居左上"),
	QStringLiteral("图片平铺"),
	QStringLiteral("图片居中"),
	};
	return Items;
}

QStringList krnl_Label::取边框列表()
{
	QStringList Items = {
	QStringLiteral("无边框"),
	QStringLiteral("凹入式"),
	QStringLiteral("凸出式"),
	QStringLiteral("浅凹入式"),
	QStringLiteral("镜框式"),
	QStringLiteral("单线边框式"),
	QStringLiteral("渐变镜框式"),
	};
	return Items;
}

QStringList krnl_Label::取横向对齐方式列表()
{
	QStringList Items = {
		QStringLiteral("左对齐"),
		QStringLiteral("居中"),
		QStringLiteral("右对齐"),
	};
	return Items;
}

QStringList krnl_Label::取纵向对齐方式列表()
{
	QStringList Items = {
		QStringLiteral("顶对齐"),
		QStringLiteral("居中"),
		QStringLiteral("底对齐"),
	};
	return Items;
}

QStringList krnl_Label::取渐变背景方式列表()
{
	QStringList Items = {
	QStringLiteral("无渐变背景"),
	QStringLiteral("从上到下"),
	QStringLiteral("从左到右"),
	QStringLiteral("从左上到右下"),
	QStringLiteral("从右上到左下"),
	QStringLiteral("从下到上"),
	QStringLiteral("从右到左"),
	QStringLiteral("从右下到左上"),
	QStringLiteral("从左下到右上"),
	};
	return Items;
}

QString krnl_Label::取图标资源路径()
{
	return QStringLiteral(":/ICO/ICO/标签.ico");
}

void krnl_Label::取控件默认附加属性(QHash<QString, QVariant>& out_data)
{
	out_data[QStringLiteral("效果")] = 0;
	out_data[QStringLiteral("渐变边框宽度")] = 8;
	out_data[QStringLiteral("边框")] = 0;
	out_data[QStringLiteral("渐变边框颜色1")] = 0x808080;
	out_data[QStringLiteral("渐变边框颜色2")] = 0xFFFFFF;
	out_data[QStringLiteral("渐变边框颜色3")] = 0x808080;
	out_data[QStringLiteral("文本颜色")] = 0x0;
	out_data[QStringLiteral("背景颜色")] = 0xFF000000;
	out_data[QStringLiteral("底图方式")] = 1;
	out_data[QStringLiteral("渐变背景方式")] = 0;
	out_data[QStringLiteral("渐变背景颜色1")] = 0x808080;
	out_data[QStringLiteral("渐变背景颜色2")] = 0xFFFFFF;
	out_data[QStringLiteral("渐变背景颜色3")] = 0x808080;
	out_data[QStringLiteral("横向对齐方式")] = 0;
	out_data[QStringLiteral("纵向对齐方式")] = 1;
	out_data[QStringLiteral("是否自动折行")] = 0;
	out_data[QStringLiteral("标题")] = QStringLiteral("标签");
}

void krnl_Label::反序列化控件附加属性(unsigned char* pUnitDataPtr, QHash<QString, QVariant>& out_data)
{
	Label_UnitData tmpData;
	tmpData.version = CDR_ReadUInt(pUnitDataPtr);
	if (tmpData.version > 2) {
		return;
	}

	tmpData.效果 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.渐变边框宽度 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.边框 = CDR_ReadUInt(pUnitDataPtr);

	tmpData.渐变边框颜色1 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.渐变边框颜色2 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.渐变边框颜色3 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.文本颜色 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.背景颜色 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.底图方式 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.渐变背景方式 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.渐变背景颜色1 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.渐变背景颜色2 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.渐变背景颜色3 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.横向对齐方式 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.纵向对齐方式 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.是否自动折行 = CDR_ReadUInt(pUnitDataPtr);

	tmpData.字体 = CDR_ReadCFreqMem(pUnitDataPtr);
	tmpData.底图 = CDR_ReadCFreqMem(pUnitDataPtr);

	tmpData.标题 = CDR_ReadCString(pUnitDataPtr);
	if (tmpData.version == 2) {
		tmpData.数据源 = CDR_ReadCString(pUnitDataPtr);
		tmpData.数据列 = CDR_ReadCString(pUnitDataPtr);
	}
	//————————————————————————————————
	out_data[QStringLiteral("version")] = tmpData.version;
	out_data[QStringLiteral("效果")] = tmpData.效果;
	out_data[QStringLiteral("渐变边框宽度")] = tmpData.渐变边框宽度;
	out_data[QStringLiteral("边框")] = tmpData.边框;
	out_data[QStringLiteral("渐变边框颜色1")] = tmpData.渐变边框颜色1;
	out_data[QStringLiteral("渐变边框颜色2")] = tmpData.渐变边框颜色2;
	out_data[QStringLiteral("渐变边框颜色3")] = tmpData.渐变边框颜色3;
	out_data[QStringLiteral("文本颜色")] = tmpData.文本颜色;
	out_data[QStringLiteral("背景颜色")] = tmpData.背景颜色;
	out_data[QStringLiteral("底图方式")] = tmpData.底图方式;
	out_data[QStringLiteral("渐变背景方式")] = tmpData.渐变背景方式;
	out_data[QStringLiteral("渐变背景颜色1")] = tmpData.渐变背景颜色1;
	out_data[QStringLiteral("渐变背景颜色2")] = tmpData.渐变背景颜色2;
	out_data[QStringLiteral("渐变背景颜色3")] = tmpData.渐变背景颜色3;
	out_data[QStringLiteral("横向对齐方式")] = tmpData.横向对齐方式;
	out_data[QStringLiteral("纵向对齐方式")] = tmpData.纵向对齐方式;
	out_data[QStringLiteral("是否自动折行")] = tmpData.是否自动折行;
	if (tmpData.字体.size()) {
		out_data[QStringLiteral("字体")] = QByteArray((char*)&tmpData.字体[0], tmpData.字体.size());
	}
	if (tmpData.底图.size()) {
		out_data[QStringLiteral("底图")] = QByteArray((char*)&tmpData.底图[0], tmpData.底图.size());
	}
	out_data[QStringLiteral("标题")] = QString::fromLocal8Bit(tmpData.标题.c_str());
	out_data[QStringLiteral("数据源")] = QString::fromLocal8Bit(tmpData.数据源.c_str());
	out_data[QStringLiteral("数据列")] = QString::fromLocal8Bit(tmpData.数据列.c_str());
	return;
}

void krnl_Label::添加渐变颜色控件(QString ProperyName, unsigned int ProperyValue)
{
	int insertRow = GuiParser::g_ControlInfoWindow->ui.ControlTable->rowCount();
	GuiParser::g_ControlInfoWindow->ui.ControlTable->insertRow(insertRow);

	GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(ProperyName,ui_LineEditor_ReadOnly));

	if (ProperyValue == 0xFF000000) {
		uint32 color = GetSysColor(COLOR_BTNFACE);
		GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("透明色"), ui_ColorDialog));
		GuiParser::g_ControlInfoWindow->ui.ControlTable->item(insertRow, COLUMN_PropertyValue)->setBackgroundColor(QColor(GetRValue(color), GetGValue(color), GetBValue(color)));
	}
	else {
		GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem("", ui_ColorDialog));
		GuiParser::g_ControlInfoWindow->ui.ControlTable->item(insertRow, COLUMN_PropertyValue)->setBackgroundColor(QColor(GetRValue(ProperyValue), GetGValue(ProperyValue), GetBValue(ProperyValue)));
	}
}

void krnl_Label::显示控件属性信息(QHash<QString, QVariant>& map_ControlData)
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
	EAppControl::添加无效控件(QStringLiteral("可停留焦点"), QStringLiteral("** 无效 **"));
	EAppControl::添加文本控件(QStringLiteral("    停留顺序"), map_ControlData[QStringLiteral("停留顺序")].toString());
	EAppControl::添加文本控件(QStringLiteral("标题"), map_ControlData[QStringLiteral("标题")].toString());
	EAppControl::添加列表控件(QStringLiteral("效果"), krnl_Label::取效果列表(), map_ControlData[QStringLiteral("效果")].toUInt());
	EAppControl::添加列表控件(QStringLiteral("边框"), krnl_Label::取边框列表(), map_ControlData[QStringLiteral("边框")].toUInt());
	EAppControl::添加文本控件(QStringLiteral("    渐变边框宽度"), map_ControlData[QStringLiteral("渐变边框宽度")].toString());
	krnl_Label::添加渐变颜色控件(QStringLiteral("渐变边框颜色1"), map_ControlData[QStringLiteral("渐变边框颜色1")].toUInt());
	krnl_Label::添加渐变颜色控件(QStringLiteral("渐变边框颜色2"), map_ControlData[QStringLiteral("渐变边框颜色2")].toUInt());
	krnl_Label::添加渐变颜色控件(QStringLiteral("渐变边框颜色3"), map_ControlData[QStringLiteral("渐变边框颜色3")].toUInt());
	EAppControl::添加字体控件(QStringLiteral("字体"), map_ControlData[QStringLiteral("字体")].toByteArray());
	EAppControl::添加颜色控件(QStringLiteral("文本颜色"), map_ControlData[QStringLiteral("文本颜色")].toUInt());
	EAppControl::添加底色控件(QStringLiteral("背景颜色"), map_ControlData[QStringLiteral("背景颜色")].toUInt());
	EAppControl::添加图片控件(QStringLiteral("底图"), map_ControlData[QStringLiteral("底图")].toByteArray());
	EAppControl::添加列表控件(QStringLiteral("    底图方式"), krnl_Label::取底图方式列表(), map_ControlData[QStringLiteral("底图方式")].toUInt());
	EAppControl::添加列表控件(QStringLiteral("渐变背景方式"), krnl_Label::取渐变背景方式列表(), map_ControlData[QStringLiteral("渐变背景方式")].toUInt());
	krnl_Label::添加渐变颜色控件(QStringLiteral("渐变背景颜色1"), map_ControlData[QStringLiteral("渐变背景颜色1")].toUInt());
	krnl_Label::添加渐变颜色控件(QStringLiteral("渐变背景颜色2"), map_ControlData[QStringLiteral("渐变背景颜色2")].toUInt());
	krnl_Label::添加渐变颜色控件(QStringLiteral("渐变背景颜色3"), map_ControlData[QStringLiteral("渐变背景颜色3")].toUInt());
	EAppControl::添加列表控件(QStringLiteral("横向对齐方式"), krnl_Label::取横向对齐方式列表(), map_ControlData[QStringLiteral("横向对齐方式")].toUInt());
	EAppControl::添加布尔控件(QStringLiteral("是否自动折行"), map_ControlData[QStringLiteral("是否自动折行3")].toBool());
	EAppControl::添加列表控件(QStringLiteral("纵向对齐方式"), krnl_Label::取纵向对齐方式列表(), map_ControlData[QStringLiteral("纵向对齐方式")].toUInt());
	EAppControl::添加文本控件(QStringLiteral("数据源"), map_ControlData[QStringLiteral("数据源")].toString());
	EAppControl::添加文本控件(QStringLiteral("数据列"), map_ControlData[QStringLiteral("数据列")].toString());

	return;
}

qstring krnl_Label::取事件名称(int eventIndex)
{
	qstring ret;
	switch (eventIndex)
	{
	case 0:
		ret = "反馈事件";
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
	case -12:
		ret = "滚轮被滚动";
		break;
	default:
		ret = "未知事件";
		break;
	}
	return ret;
}