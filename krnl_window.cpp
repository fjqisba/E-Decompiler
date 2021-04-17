#include "krnl_window.h"
#include <bytes.hpp>
#include "GuiParser.h"
#include "ControlInfoWidget.h"
#include "PropertyDelegate.h"
#include "public.h"
#include <QtWidgets/QComboBox>
#include <QVariant>

struct WinFrom_UnitData
{
	unsigned int version;
	unsigned int 边框;
	unsigned int 底图方式;
	unsigned int 底色;
	unsigned int 最大化按钮;
	unsigned int 最小化按钮;
	unsigned int 控制按钮;
	unsigned int 位置;
	unsigned int 可否移动;
	unsigned int 播放次数;
	unsigned int 回车下移焦点;
	unsigned int Esc键关闭;
	unsigned int F1键打开帮助;
	unsigned int 帮助标记值;
	unsigned int 在任务条中显示;
	unsigned int 随意移动;
	unsigned int 外形;
	unsigned int 总在最前;
	unsigned int 保持标题条激活;
	qstring 窗口类名;
	qvector<unsigned char> 底图;
	qvector<unsigned char> 图标;
	qvector<unsigned char> 背景音乐;
	qstring 标题;
	qstring 帮助文件名;
};

void krnl_window::反序列化属性(unsigned char* lpControlData, QHash<QString, QVariant>& out_data)
{
	WinFrom_UnitData tmpData;
	tmpData.version = CDR_ReadUInt(lpControlData);

	if (tmpData.version <= 6) {
		tmpData.边框 = CDR_ReadUInt(lpControlData);
		tmpData.底图方式 = CDR_ReadUInt(lpControlData);
		tmpData.底色 = CDR_ReadUInt(lpControlData);
		tmpData.最大化按钮 = CDR_ReadUInt(lpControlData);
		tmpData.最小化按钮 = CDR_ReadUInt(lpControlData);
		tmpData.控制按钮 = CDR_ReadUInt(lpControlData);
		tmpData.位置 = CDR_ReadUInt(lpControlData);
		tmpData.可否移动 = CDR_ReadUInt(lpControlData);
		tmpData.播放次数 = CDR_ReadUInt(lpControlData);
		tmpData.回车下移焦点 = CDR_ReadUInt(lpControlData);
		tmpData.Esc键关闭 = CDR_ReadUInt(lpControlData);
		tmpData.F1键打开帮助 = CDR_ReadUInt(lpControlData);
		tmpData.帮助标记值 = CDR_ReadUInt(lpControlData);

		if (tmpData.version >= 2) {
			tmpData.在任务条中显示 = CDR_ReadUInt(lpControlData);
		}
		if (tmpData.version >= 3) {
			tmpData.随意移动 = CDR_ReadUInt(lpControlData);
			tmpData.外形 = CDR_ReadUInt(lpControlData);
		}
		if (tmpData.version >= 4) {
			tmpData.总在最前 = CDR_ReadUInt(lpControlData);
		}
		if (tmpData.version >= 5) {
			tmpData.保持标题条激活 = CDR_ReadUInt(lpControlData);
		}
		if (tmpData.version == 6) {
			tmpData.窗口类名 = CDR_ReadCString(lpControlData);
		}

		tmpData.底图 = CDR_ReadCFreqMem(lpControlData);
		tmpData.图标 = CDR_ReadCFreqMem(lpControlData);
		tmpData.背景音乐 = CDR_ReadCFreqMem(lpControlData);

		tmpData.标题 = CDR_ReadCString(lpControlData);
		tmpData.帮助文件名 = CDR_ReadCString(lpControlData);
	}

	out_data[QStringLiteral("version")] = tmpData.version;
	out_data[QStringLiteral("边框")] = tmpData.边框;
	out_data[QStringLiteral("底图方式")] = tmpData.底图方式;
	out_data[QStringLiteral("底色")] = tmpData.底色;
	out_data[QStringLiteral("最大化按钮")] = tmpData.最小化按钮;
	out_data[QStringLiteral("控制按钮")] = tmpData.控制按钮;
	out_data[QStringLiteral("位置")] = tmpData.位置;
	out_data[QStringLiteral("可否移动")] = tmpData.可否移动;
	out_data[QStringLiteral("播放次数")] = tmpData.播放次数;
	out_data[QStringLiteral("回车下移焦点")] = tmpData.回车下移焦点;
	out_data[QStringLiteral("Esc键关闭")] = tmpData.Esc键关闭;
	out_data[QStringLiteral("F1键打开帮助")] = tmpData.F1键打开帮助;
	out_data[QStringLiteral("帮助标记值")] = tmpData.帮助标记值;
	out_data[QStringLiteral("在任务条中显示")] = tmpData.在任务条中显示;
	out_data[QStringLiteral("随意移动")] = tmpData.随意移动;
	out_data[QStringLiteral("外形")] = tmpData.外形;
	out_data[QStringLiteral("总在最前")] = tmpData.总在最前;
	out_data[QStringLiteral("保持标题条激活")] = tmpData.保持标题条激活;
	out_data[QStringLiteral("窗口类名")] = QString::fromLocal8Bit(tmpData.窗口类名.c_str());
	
	if (tmpData.底图.size()) {
		out_data[QStringLiteral("底图")] = QByteArray((char*)&tmpData.底图[0], tmpData.底图.size());
	}
	if (tmpData.图标.size()) {
		out_data[QStringLiteral("图标")] = QByteArray((char*)&tmpData.图标[0], tmpData.图标.size());
	}
	if (tmpData.背景音乐.size()) {
		out_data[QStringLiteral("背景音乐")] = QByteArray((char*)&tmpData.背景音乐[0], tmpData.背景音乐.size());
	}
	
	out_data[QStringLiteral("标题")] = QString::fromLocal8Bit(tmpData.标题.c_str());
	out_data[QStringLiteral("帮助文件名")] = QString::fromLocal8Bit(tmpData.帮助文件名.c_str());
	return;
}

qstring 取窗口属性_位置(unsigned int index)
{
	qstring ret;
	switch (index)
	{
	case 0:
		ret = "通常";
		break;
	case 1:
		ret = "居中";
		break;
	case 2:
		ret = "最小化";
		break;
	case 3:
		ret = "最大化";
		break;
	default:
		ret = "未知位置";
		break;
	}
	return ret;
}

qstring 取窗口属性_外形(unsigned int index)
{
	qstring ret;
	switch (index)
	{
	case 0:
		ret = "矩形";
		break;
	case 1:
		ret = "椭圆";
		break;
	case 2:
		ret = "圆角矩形";
		break;
	case 3:
		ret = "环";
		break;
	case 4:
		ret = "正三角";
		break;
	case 5:
		ret = "倒三角";
		break;
	case 6:
		ret = "左三角";
		break;
	case 7:
		ret = "右三角";
		break;
	case 8:
		ret = "平行四边形";
		break;
	case 9:
		ret = "五边形";
		break;
	case 10:
		ret = "六边形";
		break;
	case 11:
		ret = "梯形";
		break;
	case 12:
		ret = "菱形";
		break;
	case 13:
		ret = "五角星";
		break;
	case 14:
		ret = "十字星";
		break;
	case 15:
		ret = "闪电形";
		break;
	case 16:
		ret = "爆炸形1";
		break;
	case 17:
		ret = "爆炸形2";
		break;
	case 18:
		ret = "燕尾";
		break;
	case 19:
		ret = "折角矩形";
		break;
	case 20:
		ret = "左箭头";
		break;
	case 21:
		ret = "右箭头";
		break;
	case 22:
		ret = "上箭头";
		break;
	case 23:
		ret = "下箭头";
		break;
	case 24:
		ret = "左右箭头";
		break;
	case 25:
		ret = "上下箭头";
		break;
	case 26:
		ret = "十字箭头";
		break;
	case 27:
		ret = "丁子箭头";
		break;
	case 28:
		ret = "燕尾箭头";
		break;
	case 29:
		ret = "五边形箭头";
		break;
	default:
		ret = "未知箭头";
		break;
	}
	return ret;

}



qstring 取窗口属性_边框(unsigned int index)
{
	qstring ret;
	switch (index)
	{
	case 0x0:
		ret = "无边框";
		break;
	case 1:
		ret = "普通可调边框";
		break;
	case 2:
		ret = "普通固定边框";
		break;
	case 3:
		ret = "窄标题可调边框";
		break;
	case 4:
		ret = "窄标题固定边框";
		break;
	case 5:
		ret = "镜框式可调边框";
		break;
	case 6:
		ret = "镜框式固定边框";
		break;
	default:
		ret = "未知边框";
		break;
	}

	return ret;
}


qstring 取窗口属性_底图方式(unsigned int index)
{
	qstring ret;
	switch (index)
	{
	case 0:
		ret = "图片居左上";
		break;
	case 1:
		ret = "图片平铺";
		break;
	case 2:
		ret = "图片居中";
		break;
	case 3:
		ret = "缩放图片";
		break;
	default:
		ret = "未知方式";
		break;
	}
	return ret;
}

qstring 取窗口属性_播放次数(unsigned int index)
{
	qstring ret;
	switch (index)
	{
	case 0:
		ret = "循环播放";
		break;
	case 1:
		ret = "仅播放一次";
		break;
	case 2:
		ret = "不播放";
		break;
	default:
		ret = "未知播放";
		break;
	}
	return ret;
}
void krnl_window::添加底色控件(ControlInfoWidget* pWindow, unsigned int ProperyValue)
{
	int insertRow = pWindow->ui.ControlTable->rowCount();
	pWindow->ui.ControlTable->insertRow(insertRow);

	pWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("底色")));


	if (ProperyValue == 0xFF000000) {
		uint32 color = GetSysColor(COLOR_BTNFACE);
		pWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("默认底色"), ui_ColorDialog));
		pWindow->ui.ControlTable->item(insertRow, COLUMN_PropertyValue)->setBackgroundColor(QColor(GetRValue(color), GetGValue(color), GetBValue(color)));
	}
	else {
		pWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem("", ui_ColorDialog));
		pWindow->ui.ControlTable->item(insertRow, COLUMN_PropertyValue)->setBackgroundColor(QColor(GetRValue(ProperyValue), GetGValue(ProperyValue), GetBValue(ProperyValue)));
	}
}

void krnl_window::添加边框控件(ControlInfoWidget* pWindow, unsigned int ProperyValue)
{
	int insertRow = pWindow->ui.ControlTable->rowCount();
	pWindow->ui.ControlTable->insertRow(insertRow);

	pWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("边框")));
	QComboBox* pComboBox = new QComboBox();
	pComboBox->setEditable(false);

	QStringList Items = {
		QStringLiteral("无边框"),
		QStringLiteral("普通可调边框"),
		QStringLiteral("普通固定边框"),
		QStringLiteral("窄标题可调边框"),
		QStringLiteral("窄标题固定边框"),
		QStringLiteral("镜框式可调边框"),
		QStringLiteral("镜框式固定边框"),
	};
	pComboBox->addItems(Items);
	pComboBox->setCurrentIndex(ProperyValue);
	pWindow->ui.ControlTable->setCellWidget(insertRow, COLUMN_PropertyValue, pComboBox);
}

void krnl_window::取默认属性(QHash<QString, QVariant>& out_data)
{
	out_data[QStringLiteral("version")] = 6;
	out_data[QStringLiteral("边框")] = 2;
	out_data[QStringLiteral("底色")] = 0xFF000000;
	out_data[QStringLiteral("底图方式")] = 1;	
	out_data[QStringLiteral("播放次数")] = 0;
	out_data[QStringLiteral("控制按钮")] = 1;
	out_data[QStringLiteral("最大化按钮")] = 0;
	out_data[QStringLiteral("最小化按钮")] = 1;
	out_data[QStringLiteral("位置")] = 1;
	out_data[QStringLiteral("可否移动")] = 1;
	out_data[QStringLiteral("回车下移焦点")] = 0;
	out_data[QStringLiteral("Esc键关闭")] = 1;
	out_data[QStringLiteral("F1键打开帮助")] = 0;
	out_data[QStringLiteral("在任务条中显示")] = 1;
	out_data[QStringLiteral("随意移动")] = 0;
	out_data[QStringLiteral("外形")] = 0;
	out_data[QStringLiteral("总在最前")] = 0;
	out_data[QStringLiteral("保持标题条激活")] = 0;
}

void krnl_window::显示控件信息(unsigned int propertyAddr, int propertySize)
{
	QHash<QString, QVariant> map_ControlData;

	qvector<unsigned char> tmpBuf;
	tmpBuf.resize(propertySize);
	get_bytes(&tmpBuf[0], propertySize, propertyAddr);
	unsigned char* lpControlInfo = &tmpBuf[0];

	map_ControlData["控件ID"]= ReadUInt(lpControlInfo);
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



	unsigned int unKnowValueB = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	unsigned int index2 = ReadUInt(lpControlInfo);
	lpControlInfo += 4;
	lpControlInfo += (index2 * 8) + 0x14;

	//——————————————剩下的数据为UnitData——————————————
	int UnitDataSize = (&tmpBuf[0] - lpControlInfo) + propertySize;

	if (!UnitDataSize) {
		取默认属性(map_ControlData);
	}
	else {
		反序列化属性(lpControlInfo, map_ControlData);
	}

	ControlInfoWidget* pWindow = GuiParser::g_ControlInfoWindow;
	EAppControl::添加文本控件(pWindow, QStringLiteral("名称"), map_ControlData[QStringLiteral("名称")].toString());
	EAppControl::添加文本控件(pWindow, QStringLiteral("左边"), map_ControlData[QStringLiteral("左边")].toString());
	EAppControl::添加文本控件(pWindow, QStringLiteral("顶边"), map_ControlData[QStringLiteral("顶边")].toString());
	EAppControl::添加文本控件(pWindow, QStringLiteral("宽度"), map_ControlData[QStringLiteral("宽度")].toString());
	EAppControl::添加文本控件(pWindow, QStringLiteral("高度"), map_ControlData[QStringLiteral("高度")].toString());
	EAppControl::添加文本控件(pWindow, QStringLiteral("标记"), map_ControlData[QStringLiteral("标记")].toString());
	EAppControl::添加布尔控件(pWindow, QStringLiteral("可视"), map_ControlData[QStringLiteral("可视")].toBool());
	EAppControl::添加布尔控件(pWindow, QStringLiteral("禁止"), map_ControlData[QStringLiteral("禁止")].toBool());
	EAppControl::添加鼠标控件(pWindow, map_ControlData[QStringLiteral("鼠标指针")].toUInt());
	EAppControl::添加文本控件(pWindow, QStringLiteral("标题"), map_ControlData[QStringLiteral("标题")].toString());
	krnl_window::添加边框控件(pWindow, map_ControlData[QStringLiteral("边框")].toUInt());
	krnl_window::添加底色控件(pWindow, map_ControlData[QStringLiteral("底色")].toUInt());
	EAppControl::添加图片控件(pWindow, QStringLiteral("底图"), map_ControlData[QStringLiteral("底图")].toByteArray());
	return;
}


qstring krnl_window::取事件名称(int eventIndex)
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
	case 0:
		ret = "创建完毕";
		break;
	case 1:
		ret = "可否被关闭";
		break;
	case 2:
		ret = "将被销毁";
		break;
	case 3:
		ret = "位置被改变";
		break;
	case 4:
		ret = "尺寸被改变";
		break;
	case 5:
		ret = "被激活";
		break;
	case 6:
		ret = "被取消激活";
		break;
	case 7:
		ret = "空闲";
		break;
	case 8:
		ret = "首次激活";
		break;
	case 9:
		ret = "托盘事件";
		break;
	case 10:
		ret = "被显示";
		break;
	case 11:
		ret = "被隐藏";
		break;
	case 12:
		ret = "窗口可否被关闭";
		break;
	default:
		ret = "未知事件";
		break;
	}
	return ret;
}