#include "ControlInfoWidget.h"
#include "EDecompiler.h"
#include <bytes.hpp>
#include "public.h"
#include "E_WinForm.h"
#include "PropertyDelegate.h"

extern EDecompilerEngine g_MyDecompiler;

#define COLUMN_PropertyName  0
#define COLUMN_PropertyValue 1



void ControlInfoWidget::InitKernelControl_Window(mid_ControlInfo* pControl)
{
	qvector<unsigned char> tmpBuf;
	tmpBuf.resize(pControl->m_propertySize);
	get_bytes(&tmpBuf[0], pControl->m_propertySize, pControl->m_propertyAddr);
	unsigned char* lpControlInfo = &tmpBuf[0];

	ui.ControlTable->setRowCount(33);

	//窗口其实是没有名字的
	//qstring controlName;
	//controlName.sprnt("窗口_%08a", pControl->m_controlId);
	ui.ControlTable->setItem(0, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("名称")));
	ui.ControlTable->setItem(0, COLUMN_PropertyValue, new QTableWidgetItem(ui_LineEditor_Disabled));

	{
		uint32 dwControlTypeId = ReadUInt(lpControlInfo);
		lpControlInfo += 4;
		//固定的20个空字节,保留使用?
		lpControlInfo += 20;
		//无用字符串1?
		ReadStr(lpControlInfo);
		lpControlInfo += qstrlen(lpControlInfo) + 1;
		//无用字符串2?
		ReadStr(lpControlInfo);
		lpControlInfo += qstrlen(lpControlInfo) + 1;
		//存储数据?
		ReadUInt(lpControlInfo);
		lpControlInfo += 4;
	}

	int left = ReadInt(lpControlInfo);
	lpControlInfo += 4;
	ui.ControlTable->setItem(1, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("左边")));
	ui.ControlTable->setItem(1, COLUMN_PropertyValue, new QTableWidgetItem(QString::number(left)));


	int top = ReadInt(lpControlInfo);
	lpControlInfo += 4;
	ui.ControlTable->setItem(2, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("顶边")));
	ui.ControlTable->setItem(2, COLUMN_PropertyValue, new QTableWidgetItem(QString::number(top)));

	int width = ReadInt(lpControlInfo);
	lpControlInfo += 4;
	ui.ControlTable->setItem(3, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("宽度")));
	ui.ControlTable->setItem(3, COLUMN_PropertyValue, new QTableWidgetItem(QString::number(width)));

	int height = ReadInt(lpControlInfo);
	lpControlInfo += 4;
	ui.ControlTable->setItem(4, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("高度")));
	ui.ControlTable->setItem(4, COLUMN_PropertyValue, new QTableWidgetItem(QString::number(height)));

	//未知值
	lpControlInfo += 8;

	//一级索引?
	unsigned int index1 = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	//鼠标指针
	unsigned int cursorFlag = ReadUInt(lpControlInfo + 4);
	QString str_Cursor;
	switch (cursorFlag)
	{
	case 0x0:
		str_Cursor = QStringLiteral("默认型");
		break;
	case 0x00007F00:
		str_Cursor = QStringLiteral("标准箭头型");
		break;
	case 0x00007F03:
		str_Cursor = QStringLiteral("十字型");
		break;
	case 0x00007F01:
		str_Cursor = QStringLiteral("文本编辑型");
		break;
	case 0x00007F02:
		str_Cursor = QStringLiteral("沙漏型");
		break;
	case 0x00007F8B:
		str_Cursor = QStringLiteral("箭头及问号型");
		break;
	case 0x00007F8A:
		str_Cursor = QStringLiteral("箭头及沙漏型");
		break;
	case 0x00007F88:
		str_Cursor = QStringLiteral("禁止符型");
		break;
	case 0x00007F86:
		str_Cursor = QStringLiteral("四向箭头型");
		break;
	case 0x00007F83:
		str_Cursor = QStringLiteral("北<->东箭头型");
		break;
	case 0x00007F85:
		str_Cursor = QStringLiteral("北<->南箭头型");
		break;
	case 0x00007F82:
		str_Cursor = QStringLiteral("北<->西箭头型");
		break;
	case 0x00007F84:
		str_Cursor = QStringLiteral("西<->东箭头型");
		break;
	case 0x00007F04:
		str_Cursor = QStringLiteral("向上箭头型");
		break;
	case 0x00007F89:
		str_Cursor = QStringLiteral("手型");
		break;
	case 0xFFFFFFFF:
		str_Cursor = QStringLiteral("自定义型");
		break;
	default:
		str_Cursor = QStringLiteral("解析错误");
		break;
	}

	ui.ControlTable->setItem(8, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("鼠标指针")));
	ui.ControlTable->setItem(8, COLUMN_PropertyValue, new QTableWidgetItem(str_Cursor));

	//二级偏移?
	unsigned int offset2 = ReadUInt(lpControlInfo + (index1 * 4));
	lpControlInfo += index1 * 4;
	lpControlInfo += offset2 + 4;
	
	//标记
	qstring m_tag = ReadStr(lpControlInfo);
	lpControlInfo += qstrlen(lpControlInfo) + 1;
	ui.ControlTable->setItem(5, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("标记")));
	ui.ControlTable->setItem(5, COLUMN_PropertyValue, new QTableWidgetItem(QString::fromLocal8Bit(m_tag.c_str())));

	unsigned int unKnowValueA = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	unsigned int flags = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	ui.ControlTable->setItem(6, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("可视")));
	if (flags & 0x1) {
		ui.ControlTable->setItem(6, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("真")));
	}
	else {
		ui.ControlTable->setItem(6, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("假")));
	}

	ui.ControlTable->setItem(7, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("禁止")));
	if (flags & 0x2) {
		ui.ControlTable->setItem(7, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("真")));
	}
	else {
		ui.ControlTable->setItem(7, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("假")));
	}

	unsigned int unKnowValueB = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	unsigned int index2 = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	lpControlInfo += (index2 * 8) + 0x14;

	//——————————————剩下的数据为UnitData——————————————
	int UnitDataSize = (&tmpBuf[0] - lpControlInfo) + pControl->m_propertySize;

	WinFrom_UnitData WinData = {};
	Unserialize_WinFormData(lpControlInfo, &WinData);

	ui.ControlTable->setItem(9, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("标题")));
	if (WinData.标题.size()) {
		ui.ControlTable->setItem(9, COLUMN_PropertyValue, new QTableWidgetItem(QString::fromLocal8Bit(WinData.标题.c_str())));
	}
	return;
}

void ControlInfoWidget::InitKernelControl_Label(mid_ControlInfo* pControl)
{
	ui.ControlTable->insertRow(0);
	ui.ControlTable->setItem(0, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("名称")));
	ui.ControlTable->setItem(0, COLUMN_PropertyValue, new QTableWidgetItem(QString::fromLocal8Bit(pControl->m_controlName.c_str())));
}



ControlInfoWidget::ControlInfoWidget()
{
	ui.setupUi(this);

	ui.ControlTable->setColumnCount(2);
	ui.ControlTable->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.ControlTable->setSelectionBehavior(QAbstractItemView::SelectItems);
	ui.ControlTable->setItemDelegateForColumn(COLUMN_PropertyValue, new PropertyDelegate(ui.ControlTable));

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

		mid_ControlInfo currentControl;
		if (EDecompilerEngine::GetControlInfo(controlId, currentControl)) {

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

	}

}