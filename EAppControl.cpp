#include "EAppControl.h"
#include "ControlInfoWidget.h"
#include <QMap>
#include "GuiParser.h"
#include "PropertyDelegate.h"
#include <bytes.hpp>
#include "public.h"
#include "krnl_window.h"
#include "krnl_Button.h"
#include "krnl_Timer.h"
#include "krnl_ListBox.h"
#include "krnl_RadioBox.h"
#include "krnl_PicBtn.h"
#include "krnl_EditBox.h"
#include "krnl_PicBox.h"
#include "krnl_ShapeBox.h"
#include "krnl_CheckBox.h"
#include "krnl_DropTarget.h"
#include "EAppControlFactory.h"

unsigned char* EAppControl::GetUnitDataPtr(unsigned char* propertyAddr)
{
	unsigned char* ret = propertyAddr;
	ret += 0x18;
	ret += qstrlen(ret) + 1;
	ret += qstrlen(ret) + 1;
	ret += 0x1C;
	ret += (ReadUInt(ret) + 1) * 4;
	ret += ReadUInt(ret) + 4;
	ret += qstrlen(ret) + 1;
	ret += 0xC;
	ret += ReadUInt(ret) * 8;
	ret += 0x18;
	return ret;
}

QString 取鼠标指针属性(unsigned int index)
{
	QString ret;
	switch (index)
	{
	case 0x0:
		ret = QStringLiteral("默认型");
		break;
	case 0x00007F00:
		ret = QStringLiteral("标准箭头型");
		break;
	case 0x00007F03:
		ret = QStringLiteral("十字型");
		break;
	case 0x00007F01:
		ret = QStringLiteral("文本编辑型");
		break;
	case 0x00007F02:
		ret = QStringLiteral("沙漏型");
		break;
	case 0x00007F8B:
		ret = QStringLiteral("箭头及问号型");
		break;
	case 0x00007F8A:
		ret = QStringLiteral("箭头及沙漏型");
		break;
	case 0x00007F88:
		ret = QStringLiteral("禁止符型");
		break;
	case 0x00007F86:
		ret = QStringLiteral("四向箭头型");
		break;
	case 0x00007F83:
		ret = QStringLiteral("北<->东箭头型");
		break;
	case 0x00007F85:
		ret = QStringLiteral("北<->南箭头型");
		break;
	case 0x00007F82:
		ret = QStringLiteral("北<->西箭头型");
		break;
	case 0x00007F84:
		ret = QStringLiteral("西<->东箭头型");
		break;
	case 0x00007F04:
		ret = QStringLiteral("向上箭头型");
		break;
	case 0x00007F89:
		ret = QStringLiteral("手型");
		break;
	case 0xFFFFFFFF:
		ret = QStringLiteral("自定义型");
		break;
	default:
		ret = QStringLiteral("未知");
		break;
	}

	return ret;
}

void EAppControl::解析控件基础属性(unsigned char* lpControlInfo, QHash<QString, QVariant>& map_ControlData)
{
	map_ControlData["控件ID"] = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	//固定的20个空字节,保留使用?
	lpControlInfo += 20;

	map_ControlData[QStringLiteral("名称")] = QString::fromLocal8Bit(ReadStr(lpControlInfo).c_str());
	lpControlInfo += qstrlen(lpControlInfo) + 1;

	//无用字符串?
	ReadStr(lpControlInfo);
	lpControlInfo += qstrlen(lpControlInfo) + 1;

	//存储数据?
	ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	map_ControlData[QStringLiteral("左边")] = ReadInt(lpControlInfo);
	lpControlInfo += 4;

	map_ControlData[QStringLiteral("顶边")] = ReadInt(lpControlInfo);
	lpControlInfo += 4;

	map_ControlData[QStringLiteral("宽度")] = ReadInt(lpControlInfo);
	lpControlInfo += 4;

	map_ControlData[QStringLiteral("高度")] = ReadInt(lpControlInfo);
	lpControlInfo += 4;

	//值为0,用来存储LoadCursorA返回的句柄值的
	lpControlInfo += 4;

	//父控件ID
	map_ControlData[QStringLiteral("父控件ID")] = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	//子控件数目
	unsigned int childControlCount = ReadUInt(lpControlInfo);
	lpControlInfo += 4;
	for (unsigned int n = 0; n < childControlCount; ++n) {
		unsigned int tmpChildControlId = ReadUInt(lpControlInfo);
		lpControlInfo += 4;
	}

	//鼠标指针大小
	unsigned int CursorSize = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	map_ControlData[QStringLiteral("鼠标指针")] = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	if (CursorSize > 4) {
		map_ControlData[QStringLiteral("鼠标指针数据")] = QByteArray((char*)lpControlInfo, CursorSize);
		lpControlInfo += CursorSize - 4;
	}

	map_ControlData[QStringLiteral("标记")] = QString::fromLocal8Bit(ReadStr(lpControlInfo).c_str());
	lpControlInfo += qstrlen(lpControlInfo) + 1;

	//未知的值
	unsigned int unKnowValueA = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	unsigned int windowFlags = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	map_ControlData[QStringLiteral("可视")] = windowFlags & 0x1;
	map_ControlData[QStringLiteral("禁止")] = windowFlags & 0x2;
	map_ControlData[QStringLiteral("可停留焦点")] = windowFlags & 0x4;

	map_ControlData[QStringLiteral("停留顺序")] = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	unsigned int index2 = ReadUInt(lpControlInfo);
	lpControlInfo += 4;
	lpControlInfo += (index2 * 8) + 0x14;

	return;
}

QString EAppControl::取图标资源路径()
{
	return QStringLiteral(":/ICO/ICO/Unknow.ico");
}

void EAppControl::显示控件信息(ControlType_t type, unsigned int propertyAddr, int propertySize)
{
	QHash<QString, QVariant> map_ControlData;
	qvector<unsigned char> tmpBuf;
	tmpBuf.resize(propertySize);
	get_bytes(&tmpBuf[0], propertySize, propertyAddr);

	if (type == krnl_menu) {

	}
	else {
		EAppControl::解析控件基础属性(&tmpBuf[0], map_ControlData);
	}
	
	EAppControl* pEAppControl = EAppControlFactory::getEAppControl(type);
	if (!pEAppControl) {
		return;
	}



	unsigned char* pUnitDataPtr = EAppControl::GetUnitDataPtr(&tmpBuf[0]);
	int UnitDataSize = (&tmpBuf[0] - pUnitDataPtr) + propertySize;
	if (!UnitDataSize) {
		pEAppControl->取控件默认附加属性(map_ControlData);
	}
	else {
		pEAppControl->反序列化控件附加属性(pUnitDataPtr, map_ControlData);
	}

	pEAppControl->显示控件属性信息(map_ControlData);
	return;
}

void EAppControl::添加文本控件(QString PropertyName, QString PropertyValue)
{
	int insertRow = GuiParser::g_ControlInfoWindow->ui.ControlTable->rowCount();
	GuiParser::g_ControlInfoWindow->ui.ControlTable->insertRow(insertRow);

	GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(PropertyName, ui_LineEditor_ReadOnly));
	GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem(PropertyValue));
}

void EAppControl::添加列表控件(QString PropertyName, QStringList& PropertyList, unsigned int PropertyValue)
{
	int insertRow = GuiParser::g_ControlInfoWindow->ui.ControlTable->rowCount();
	GuiParser::g_ControlInfoWindow->ui.ControlTable->insertRow(insertRow);
	GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(PropertyName, ui_LineEditor_ReadOnly));
	QComboBox* pComboBox = new QComboBox();
	pComboBox->setEditable(false);

	pComboBox->addItems(PropertyList);
	pComboBox->setCurrentIndex(PropertyValue);
	GuiParser::g_ControlInfoWindow->ui.ControlTable->setCellWidget(insertRow, COLUMN_PropertyValue, pComboBox);
	return;
}

void EAppControl::添加无效控件(QString PropertyName,QString PropertyValue)
{
	int insertRow = GuiParser::g_ControlInfoWindow->ui.ControlTable->rowCount();
	GuiParser::g_ControlInfoWindow->ui.ControlTable->insertRow(insertRow);
	GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(PropertyName, ui_LineEditor_ReadOnly));
	GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem(PropertyValue, ui_LineEditor_Disabled));
	return;
}

void EAppControl::添加字体控件(QString PropertyName, QByteArray& PropertyValue)
{
	int insertRow = GuiParser::g_ControlInfoWindow->ui.ControlTable->rowCount();
	GuiParser::g_ControlInfoWindow->ui.ControlTable->insertRow(insertRow);
	GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(PropertyName, ui_LineEditor_ReadOnly));

	if (PropertyValue.size()) {
		GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("有数据"), ui_LineEditor_ReadOnly));
	}
	else {
		GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral(""), ui_LineEditor_ReadOnly));
	}
}

void EAppControl::添加底色控件(QString propertyName, unsigned int ProperyValue)
{
	int insertRow = GuiParser::g_ControlInfoWindow->ui.ControlTable->rowCount();
	GuiParser::g_ControlInfoWindow->ui.ControlTable->insertRow(insertRow);

	GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(propertyName, ui_LineEditor_ReadOnly));

	if (ProperyValue == 0xFF000000) {
		uint32 color = GetSysColor(COLOR_BTNFACE);
		GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("默认底色"), ui_ColorDialog));
		GuiParser::g_ControlInfoWindow->ui.ControlTable->item(insertRow, COLUMN_PropertyValue)->setBackgroundColor(QColor(GetRValue(color), GetGValue(color), GetBValue(color)));
	}
	else {
		GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem("", ui_ColorDialog));
		GuiParser::g_ControlInfoWindow->ui.ControlTable->item(insertRow, COLUMN_PropertyValue)->setBackgroundColor(QColor(GetRValue(ProperyValue), GetGValue(ProperyValue), GetBValue(ProperyValue)));
	}
}

void EAppControl::添加颜色控件(QString PropertyName, unsigned int PropertyValue)
{
	int insertRow = GuiParser::g_ControlInfoWindow->ui.ControlTable->rowCount();
	GuiParser::g_ControlInfoWindow->ui.ControlTable->insertRow(insertRow);
	GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(PropertyName, ui_LineEditor_ReadOnly));
	GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral(""), ui_ColorDialog));
	if (PropertyValue == 0xFF000000) {
		uint32 color = GetSysColor(COLOR_BTNFACE);
		GuiParser::g_ControlInfoWindow->ui.ControlTable->item(insertRow, COLUMN_PropertyValue)->setBackgroundColor(QColor(GetRValue(color), GetGValue(color), GetBValue(color)));
	}
	else {
		GuiParser::g_ControlInfoWindow->ui.ControlTable->item(insertRow, COLUMN_PropertyValue)->setBackgroundColor(QColor(GetRValue(PropertyValue), GetGValue(PropertyValue), GetBValue(PropertyValue)));
	}
}

void EAppControl::添加图片控件(QString PropertyName, QByteArray& PropertyValue)
{
	int insertRow = GuiParser::g_ControlInfoWindow->ui.ControlTable->rowCount();
	GuiParser::g_ControlInfoWindow->ui.ControlTable->insertRow(insertRow);
	GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(PropertyName));

	if (PropertyValue.size()) {
		GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("有数据"), ui_ImageBox));
	}
	else {
		GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral(""), ui_ImageBox));
	}

	QImage image = QImage::fromData(PropertyValue);
	GuiParser::g_ControlInfoWindow->ui.ControlTable->item(insertRow, COLUMN_PropertyValue)->setData(Qt::UserRole, image);
	return;
}

void EAppControl::添加鼠标控件(unsigned int ProperyValue)
{
	int insertRow = GuiParser::g_ControlInfoWindow->ui.ControlTable->rowCount();
	GuiParser::g_ControlInfoWindow->ui.ControlTable->insertRow(insertRow);

	GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("鼠标指针")));
	QComboBox* pComboBox = new QComboBox();
	pComboBox->setEditable(false);

	QStringList Items = {
		QStringLiteral("默认型"),
		QStringLiteral("标准箭头型"),
		QStringLiteral("十字型"),
		QStringLiteral("文本编辑型"),
		QStringLiteral("沙漏型"),
		QStringLiteral("箭头及问号型"),
		QStringLiteral("箭头及沙漏型"),
		QStringLiteral("禁止符型"),
		QStringLiteral("四向箭头型"),
		QStringLiteral("北<->东箭头型"),
		QStringLiteral("北<->南箭头型"),
		QStringLiteral("北<->西箭头型"),
		QStringLiteral("西<->东箭头型"),
		QStringLiteral("向上箭头型"),
		QStringLiteral("手型"),
		QStringLiteral("自定义型"),
	};
	pComboBox->addItems(Items);
	pComboBox->setCurrentText(取鼠标指针属性(ProperyValue));
	GuiParser::g_ControlInfoWindow->ui.ControlTable->setCellWidget(insertRow, COLUMN_PropertyValue, pComboBox);
}

void EAppControl::添加布尔控件(QString PropertyName, bool ProperyValue)
{
	int insertRow = GuiParser::g_ControlInfoWindow->ui.ControlTable->rowCount();
	GuiParser::g_ControlInfoWindow->ui.ControlTable->insertRow(insertRow);

	GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(PropertyName, ui_LineEditor_ReadOnly));
	QComboBox* pComboBox = new QComboBox();
	pComboBox->setEditable(false);
	pComboBox->addItem(QStringLiteral("真"));
	pComboBox->addItem(QStringLiteral("假"));
	pComboBox->setCurrentIndex(!ProperyValue);
	GuiParser::g_ControlInfoWindow->ui.ControlTable->setCellWidget(insertRow, COLUMN_PropertyValue, pComboBox);
}