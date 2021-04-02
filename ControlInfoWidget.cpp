#include "ControlInfoWidget.h"
#include "EDecompiler.h"


extern EDecompilerEngine g_MyDecompiler;

#define COLUMN_PropertyName  0
#define COLUMN_PropertyValue 1

void ControlInfoWidget::InitKernelControl_Window(mid_ControlInfo* pControl)
{
	ui.ControlTable->insertRow(0);
	ui.ControlTable->setItem(0, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("名称")));
	ui.ControlTable->setItem(0, COLUMN_PropertyValue, new QTableWidgetItem(pControl->m_basicProperty.m_controlName.c_str()));
}

void ControlInfoWidget::InitKernelControl_Label(mid_ControlInfo* pControl)
{
	ui.ControlTable->insertRow(0);
	ui.ControlTable->setItem(0, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("名称")));
	ui.ControlTable->setItem(0, COLUMN_PropertyValue, new QTableWidgetItem(QString::fromLocal8Bit(pControl->m_basicProperty.m_controlName.c_str())));
}



ControlInfoWidget::ControlInfoWidget()
{
	ui.setupUi(this);

	ui.ControlTable->setColumnCount(2);
	ui.ControlTable->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.ControlTable->setSelectionBehavior(QAbstractItemView::SelectItems);

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
		QMap<unsigned int, ControlIndex>::iterator it = g_MyDecompiler.m_eAppInfo.mMap_ControlIndex.find(controlId);
		if (it != g_MyDecompiler.m_eAppInfo.mMap_ControlIndex.end()) {
			mid_ControlInfo& currentControl = g_MyDecompiler.m_eAppInfo.mVec_GuiInfo[it->nWindowIndex].mVec_ControlInfo[it->nControlIndex];

			ui.groupBox->setTitle(QString::fromLocal8Bit(currentControl.m_controlTypeName.c_str()));

			ControlType_t controlType = EDecompilerEngine::GetControlType(currentControl.m_controlTypeId);
			switch (controlType)
			{
			case EC_UnknownControl:
				break;
			case EC_Window:
				InitKernelControl_Window(&currentControl);
				break;
			case EC_Label:
				InitKernelControl_Label(&currentControl);
				break;
			default:
				break;
			}
		}

		//unsigned int tmpWindowId = parentItem->data(0, Qt::UserRole).toUInt();
		//for (unsigned int nWindowIndex = 0; nWindowIndex < g_MyDecompiler.m_eAppInfo.mVec_GuiInfo.size(); ++nWindowIndex) {
		//	if (g_MyDecompiler.m_eAppInfo.mVec_GuiInfo[nWindowIndex].m_windowId == tmpWindowId) {
		//		qvector<mid_ControlInfo>& vec_ControlInfo = g_MyDecompiler.m_eAppInfo.mVec_GuiInfo[nWindowIndex].mVec_ControlInfo;
		//		for (unsigned int nControlIndex = 0; nControlIndex < vec_ControlInfo.size(); ++nControlIndex) {
		//			if (vec_ControlInfo[nControlIndex].m_controlId == item->data(0, Qt::UserRole).toUInt()) {

		//				//找到具体的控件了
		//				ui.groupBox->setTitle(QString::fromLocal8Bit(vec_ControlInfo[nControlIndex].m_controlTypeName.c_str()));
		//				int a = 0;
		//			}
		//		}
		//	}
		
	}
	int a = 0;
}