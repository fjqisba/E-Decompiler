#include "EAppControl.h"
#include "ControlInfoWidget.h"
#include <QMap>
#include "GuiParser.h"
#include "PropertyDelegate.h"
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

qstring EAppControl::取事件名称(ControlType_t type, int eventIndex)
{
	qstring ret;

	static QMap<ControlType_t, qstring(*)(int)> map_eventFunc;
	if (!map_eventFunc.size()) {
		map_eventFunc[krnl_window] = krnl_window::取事件名称;
		map_eventFunc[krnl_EditBox] = krnl_EditBox::取事件名称;
		map_eventFunc[krnl_PicBox] = krnl_PicBox::取事件名称;
		map_eventFunc[krnl_ShapeBox] = krnl_ShapeBox::取事件名称;
		map_eventFunc[krnl_CheckBox] = krnl_CheckBox::取事件名称;
		map_eventFunc[krnl_Button] = krnl_Button::取事件名称;
		map_eventFunc[krnl_Timer] = krnl_Timer::取事件名称;
		map_eventFunc[krnl_ListBox] = krnl_ListBox::取事件名称;
		map_eventFunc[krnl_RadioBox] = krnl_RadioBox::取事件名称;
		map_eventFunc[krnl_PicBtn] = krnl_PicBtn::取事件名称;


		map_eventFunc[krnl_DropTarget] = krnl_DropTarget::取事件名称;
	}

	QMap<ControlType_t, qstring(*)(int)>::iterator it = map_eventFunc.find(type);
	if (it != map_eventFunc.end()) {
		ret = it.value()(eventIndex);
	}
	return ret;
}

void EAppControl::显示控件信息(ControlType_t type, unsigned int propertyAddr, int propertySize)
{
	static QMap<ControlType_t, void(*)(unsigned int, int)> map_ShowGuiFunc;
	if (!map_ShowGuiFunc.size()) {
		map_ShowGuiFunc[krnl_window] = krnl_window::显示控件信息;

	}

	QMap<ControlType_t, void(*)(unsigned int, int)>::iterator it = map_ShowGuiFunc.find(type);
	if (it != map_ShowGuiFunc.end()) {
		it.value()(propertyAddr, propertySize);
	}
	return;
}

void EAppControl::添加文本控件(ControlInfoWidget* pWindow, QString PropertyName, QString PropertyValue)
{
	int insertRow = pWindow->ui.ControlTable->rowCount();
	pWindow->ui.ControlTable->insertRow(insertRow);

	pWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(PropertyName));
	pWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem(PropertyValue));
}

void EAppControl::添加列表控件(ControlInfoWidget* pWindow, QString PropertyName, unsigned int PropertyValue)
{

}

void EAppControl::添加图片控件(ControlInfoWidget* pWindow, QString PropertyName, QByteArray& PropertyValue)
{
	int insertRow = pWindow->ui.ControlTable->rowCount();
	pWindow->ui.ControlTable->insertRow(insertRow);
	pWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(PropertyName));

	if (PropertyValue.size()) {
		pWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("有数据"), ui_ImageBox));
	}
	else {
		pWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral(""), ui_ImageBox));
	}

	QImage image = QImage::fromData(PropertyValue);
	pWindow->ui.ControlTable->item(insertRow, COLUMN_PropertyValue)->setData(Qt::UserRole, image);
	return;
}

void EAppControl::添加鼠标控件(ControlInfoWidget* pWindow, unsigned int ProperyValue)
{
	int insertRow = pWindow->ui.ControlTable->rowCount();
	pWindow->ui.ControlTable->insertRow(insertRow);

	pWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("鼠标指针")));
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
	pWindow->ui.ControlTable->setCellWidget(insertRow, COLUMN_PropertyValue, pComboBox);
}

void EAppControl::添加布尔控件(ControlInfoWidget* pWindow, QString PropertyName, bool ProperyValue)
{
	int insertRow = pWindow->ui.ControlTable->rowCount();
	pWindow->ui.ControlTable->insertRow(insertRow);

	pWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(PropertyName));
	QComboBox* pComboBox = new QComboBox();
	pComboBox->setEditable(false);
	pComboBox->addItem(QStringLiteral("真"));
	pComboBox->addItem(QStringLiteral("假"));
	pComboBox->setCurrentIndex(!ProperyValue);
	pWindow->ui.ControlTable->setCellWidget(insertRow, COLUMN_PropertyValue, pComboBox);
}