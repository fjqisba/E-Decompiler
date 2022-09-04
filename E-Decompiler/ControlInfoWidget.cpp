#include "ControlInfoWidget.h"

#include <bytes.hpp>
#include "PropertyDelegate.h"


#define COLUMN_PropertyName  0
#define COLUMN_PropertyValue 1


QString Control_GetBoolStr(unsigned int value)
{
	QString ret = QStringLiteral("真");
	if (!value) {
		ret = QStringLiteral("假");
	}
	return ret;
}

ControlInfoWidget::ControlInfoWidget()
{
	ui.setupUi(this);

	ui.ControlTable->setColumnCount(2);
	ui.ControlTable->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.ControlTable->setSelectionBehavior(QAbstractItemView::SelectItems);
	ui.ControlTable->setItemDelegate(new PropertyDelegate(ui.ControlTable));
	ui.ControlTable->setColumnWidth(1, 200);   //设置第二列宽度

	connect(ui.treeWidget, &QTreeWidget::itemClicked, this, [&](QTreeWidgetItem* item, int column) {
		on_controlClicked(item, column);
		});
}

ControlInfoWidget::~ControlInfoWidget()
{

}

void ControlInfoWidget::on_controlClicked(QTreeWidgetItem* item, int column)
{
	ui.ControlTable->setRowCount(0);
	ui.ControlTable->clearContents();

	QTreeWidgetItem* parentItem = item->parent();
	if (!item->parent()) {
		//点击的是最上层节点
		ui.groupBox->setTitle(QStringLiteral("窗口"));
	}
	else {
		unsigned int controlId = item->data(0, Qt::UserRole).toUInt();

		//GuiParser::mid_ControlInfo currentControl;
		//if (GuiParser::GetControlInfo(controlId, currentControl)) {
		//	ui.groupBox->setTitle(QString::fromLocal8Bit(currentControl.m_controlTypeName.c_str()));
			//eSymbol_ControlType controlType = GuiParser::GetControlType(currentControl.m_controlTypeId);
			//EAppControl::显示控件信息(controlType, currentControl.m_propertyAddr, currentControl.m_propertySize);
		//}
	}

}