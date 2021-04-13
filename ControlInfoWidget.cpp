#include "ControlInfoWidget.h"
#include "GuiParser.h"
#include <bytes.hpp>
#include "public.h"
#include "krnl_window.h"
#include "krnl_Label.h"
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

void ControlInfoWidget::InitKernelControl_Window(unsigned int propertyAddr, int propertySize)
{	
	qvector<unsigned char> tmpBuf;
	tmpBuf.resize(propertySize);
	get_bytes(&tmpBuf[0], propertySize, propertyAddr);
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
		//名称
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

	ui.ControlTable->setItem(8, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("鼠标指针")));
	ui.ControlTable->setItem(8, COLUMN_PropertyValue, new QTableWidgetItem(QString::fromLocal8Bit(取窗口属性_鼠标指针(cursorFlag).c_str())));

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
	int UnitDataSize = (&tmpBuf[0] - lpControlInfo) + propertySize;

	WinFrom_UnitData WinData = {};
	if (!UnitDataSize) {
		取窗口属性_默认(&WinData);
	}
	else {
		Unserialize_WinFormData(lpControlInfo, &WinData);
	}
	
	ui.ControlTable->setItem(9, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("标题")));
	if (WinData.标题.size()) {
		ui.ControlTable->setItem(9, COLUMN_PropertyValue, new QTableWidgetItem(QString::fromLocal8Bit(WinData.标题.c_str())));
	}

	ui.ControlTable->setItem(10, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("边框")));
	ui.ControlTable->setItem(10, COLUMN_PropertyValue, new QTableWidgetItem(QString::fromLocal8Bit(取窗口属性_边框(WinData.边框).c_str())));

	ui.ControlTable->setItem(11, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("底色")));
	if (WinData.底色 == 0xFF000000) {
		uint32 color = GetSysColor(COLOR_BTNFACE);
		ui.ControlTable->setItem(11, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("默认底色"), ui_ColorDialog));
		ui.ControlTable->item(11, COLUMN_PropertyValue)->setBackgroundColor(QColor(GetRValue(color), GetGValue(color), GetBValue(color)));
	}
	else {
		ui.ControlTable->setItem(11, COLUMN_PropertyValue, new QTableWidgetItem("", ui_ColorDialog));
		ui.ControlTable->item(11, COLUMN_PropertyValue)->setBackgroundColor(QColor(GetRValue(WinData.底色), GetGValue(WinData.底色), GetBValue(WinData.底色)));
	}

	ui.ControlTable->setItem(12, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("底图")));
	if (WinData.底图.size()) {
		ui.ControlTable->setItem(12, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("有数据")));
		//懒得显图了...
	}

	ui.ControlTable->setItem(13, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("    底图方式")));
	ui.ControlTable->setItem(13, COLUMN_PropertyValue, new QTableWidgetItem(QString::fromLocal8Bit(取窗口属性_底图方式(WinData.底图方式).c_str())));

	ui.ControlTable->setItem(14, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("背景音乐")));
	if (WinData.背景音乐.size()) {
		ui.ControlTable->setItem(14, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("有数据")));
	}

	ui.ControlTable->setItem(15, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("    播放次数")));
	ui.ControlTable->setItem(15, COLUMN_PropertyValue, new QTableWidgetItem(QString::fromLocal8Bit(取窗口属性_播放次数(WinData.播放次数).c_str())));

	ui.ControlTable->setItem(16, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("控制按钮")));
	ui.ControlTable->setItem(16, COLUMN_PropertyValue, new QTableWidgetItem(Control_GetBoolStr(WinData.控制按钮)));

	ui.ControlTable->setItem(17, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("    最大化按钮")));
	ui.ControlTable->setItem(17, COLUMN_PropertyValue, new QTableWidgetItem(Control_GetBoolStr(WinData.最大化按钮)));

	ui.ControlTable->setItem(18, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("    最小化按钮")));
	ui.ControlTable->setItem(18, COLUMN_PropertyValue, new QTableWidgetItem(Control_GetBoolStr(WinData.最小化按钮)));

	ui.ControlTable->setItem(19, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("位置")));
	ui.ControlTable->setItem(19, COLUMN_PropertyValue, new QTableWidgetItem(QString::fromLocal8Bit(取窗口属性_位置(WinData.位置).c_str())));
	
	ui.ControlTable->setItem(20, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("可否移动")));
	ui.ControlTable->setItem(20, COLUMN_PropertyValue, new QTableWidgetItem(Control_GetBoolStr(WinData.可否移动)));

	ui.ControlTable->setItem(21, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("图标")));
	if (WinData.图标.size()) {
		ui.ControlTable->setItem(21, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("有数据")));
	}

	ui.ControlTable->setItem(22, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("回车下移焦点")));
	ui.ControlTable->setItem(22, COLUMN_PropertyValue, new QTableWidgetItem(Control_GetBoolStr(WinData.回车下移焦点)));

	ui.ControlTable->setItem(23, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("Esc键关闭")));
	ui.ControlTable->setItem(23, COLUMN_PropertyValue, new QTableWidgetItem(Control_GetBoolStr(WinData.Esc键关闭)));

	ui.ControlTable->setItem(24, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("F1键打开帮助")));
	ui.ControlTable->setItem(24, COLUMN_PropertyValue, new QTableWidgetItem(Control_GetBoolStr(WinData.F1键打开帮助)));

	ui.ControlTable->setItem(25, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("    帮助文件名")));
	ui.ControlTable->setItem(25, COLUMN_PropertyValue, new QTableWidgetItem(QString::fromLocal8Bit(WinData.帮助文件名.c_str())));

	ui.ControlTable->setItem(26, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("    帮助标记值")));
	ui.ControlTable->setItem(26, COLUMN_PropertyValue, new QTableWidgetItem(QString::number(WinData.帮助标记值)));

	ui.ControlTable->setItem(27, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("在任务条中显示")));
	ui.ControlTable->setItem(27, COLUMN_PropertyValue, new QTableWidgetItem(Control_GetBoolStr(WinData.在任务条中显示)));
	
	ui.ControlTable->setItem(28, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("随意移动")));
	ui.ControlTable->setItem(28, COLUMN_PropertyValue, new QTableWidgetItem(Control_GetBoolStr(WinData.随意移动)));
	
	ui.ControlTable->setItem(29, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("外形")));
	ui.ControlTable->setItem(29, COLUMN_PropertyValue, new QTableWidgetItem(QString::fromLocal8Bit(取窗口属性_外形(WinData.外形).c_str())));

	ui.ControlTable->setItem(30, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("总在最前")));
	ui.ControlTable->setItem(30, COLUMN_PropertyValue, new QTableWidgetItem(Control_GetBoolStr(WinData.总在最前)));

	ui.ControlTable->setItem(31, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("保持标题条激活")));
	ui.ControlTable->setItem(31, COLUMN_PropertyValue, new QTableWidgetItem(Control_GetBoolStr(WinData.保持标题条激活)));

	ui.ControlTable->setItem(32, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("窗口类名")));
	ui.ControlTable->setItem(32, COLUMN_PropertyValue, new QTableWidgetItem(QString::fromLocal8Bit(WinData.窗口类名.c_str())));
	return;
}

void ControlInfoWidget::InitKernelControl_Label(unsigned int propertyAddr, int propertySize)
{
	qvector<unsigned char> tmpBuf;
	tmpBuf.resize(propertySize);
	get_bytes(&tmpBuf[0], propertySize, propertyAddr);
	unsigned char* lpControlInfo = &tmpBuf[0];

	ui.ControlTable->setRowCount(33);

	ui.ControlTable->setItem(0, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("名称")));
	//ui.ControlTable->setItem(0, COLUMN_PropertyValue, new QTableWidgetItem(QString::fromLocal8Bit(pControl->m_controlName.c_str()), ui_LineEditor_ReadOnly));

	{
		uint32 dwControlTypeId = ReadUInt(lpControlInfo);
		lpControlInfo += 4;
		//固定的20个空字节,保留使用?
		lpControlInfo += 20;
		//名称
		qstring 名称 = ReadStr(lpControlInfo);
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
	ui.ControlTable->setItem(8, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("鼠标指针")));
	ui.ControlTable->setItem(8, COLUMN_PropertyValue, new QTableWidgetItem(QString::fromLocal8Bit(取窗口属性_鼠标指针(cursorFlag).c_str())));

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
	int UnitDataSize = (&tmpBuf[0] - lpControlInfo) + propertySize;

	Label_UnitData LabelData = {};
	if (!UnitDataSize) {
		
	}
	else {
		Unserialize_LabelData(lpControlInfo, &LabelData);
	}

	return;
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

		GuiParser::mid_ControlInfo currentControl;
		if (GuiParser::GetControlInfo(controlId, currentControl)) {

			ui.groupBox->setTitle(QString::fromLocal8Bit(currentControl.m_controlTypeName.c_str()));
			ControlType_t controlType = GuiParser::GetControlType(currentControl.m_controlTypeId);
			ea_t propertyAddr = currentControl.m_propertyAddr;
			int propertySize = currentControl.m_propertySize;
			switch (controlType)
			{
			case krnl_window:
				InitKernelControl_Window(propertyAddr, propertySize);
				break;
			case krnl_Label:
				InitKernelControl_Label(propertyAddr, propertySize);
				break;
			default:
				break;
			}
		}

	}

}