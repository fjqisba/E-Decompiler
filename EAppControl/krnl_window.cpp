#include "krnl_window.h"
#include <bytes.hpp>
#include "../GuiParser.h"
#include "../ControlInfoWidget.h"
#include "../PropertyDelegate.h"
#include "../common/public.h"
#include <QtWidgets/QComboBox>

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
	unsigned int 帮助标志值;
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

QStringList 取底图方式列表()
{
	QStringList Items = {
	QStringLiteral("图片居左上"),
	QStringLiteral("图片平铺"),
	QStringLiteral("图片居中"),
	QStringLiteral("缩放图片"),
	};
	return Items;
}

QStringList 取位置列表()
{
	QStringList Items = {
		QStringLiteral("通常"),
		QStringLiteral("居中"),
		QStringLiteral("最小化"),
		QStringLiteral("最大化"),
	};
	return Items;
}

QStringList 取外形列表()
{
	QStringList Items = {
		QStringLiteral("矩形"),
		QStringLiteral("椭圆"),
		QStringLiteral("圆角矩形"),
		QStringLiteral("环"),
		QStringLiteral("正三角"),
		QStringLiteral("倒三角"),
		QStringLiteral("左三角"),
		QStringLiteral("右三角"),
		QStringLiteral("平行四边形"),
		QStringLiteral("五边形"),
		QStringLiteral("六边形"),
		QStringLiteral("梯形"),
		QStringLiteral("菱形"),
		QStringLiteral("五角星"),
		QStringLiteral("十字星"),
		QStringLiteral("闪电形"),
		QStringLiteral("爆炸形1"),
		QStringLiteral("爆炸形2"),
		QStringLiteral("燕尾"),
		QStringLiteral("折角矩形"),
		QStringLiteral("左箭头"),
		QStringLiteral("右箭头"),
		QStringLiteral("上箭头"),
		QStringLiteral("下箭头"),
		QStringLiteral("左右箭头"),
		QStringLiteral("上下箭头"),
		QStringLiteral("十字箭头"),
		QStringLiteral("丁子箭头"),
		QStringLiteral("燕尾箭头"),
		QStringLiteral("五边形箭头"),
	};
	return Items;
}

QStringList 取播放次数列表()
{
	QStringList Items = {
		QStringLiteral("循环播放"),
		QStringLiteral("仅播放一次"),
		QStringLiteral("不播放"),
	};
	return Items;
}

QStringList 取边框列表()
{
	QStringList Items = {
		QStringLiteral("无边框"),
		QStringLiteral("普通可调边框"),
		QStringLiteral("普通固定边框"),
		QStringLiteral("窄标题可调边框"),
		QStringLiteral("窄标题固定边框"),
		QStringLiteral("镜框式可调边框"),
		QStringLiteral("镜框式固定边框"),
	};
	return Items;
}

void krnl_window::添加背景音乐控件(QByteArray& ProperyValue)
{
	int insertRow = GuiParser::g_ControlInfoWindow->ui.ControlTable->rowCount();
	GuiParser::g_ControlInfoWindow->ui.ControlTable->insertRow(insertRow);

	GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyName, new QTableWidgetItem(QStringLiteral("背景音乐")));
	if (ProperyValue.size()) {
		GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("有数据")));
	}
	else {
		GuiParser::g_ControlInfoWindow->ui.ControlTable->setItem(insertRow, COLUMN_PropertyValue, new QTableWidgetItem(QStringLiteral("")));
	}
	return;
}


QString krnl_window::取图标资源路径()
{
	return QStringLiteral(":/ICO/ICO/Window.ico");
}

void krnl_window::取控件默认附加属性(QHash<QString, QVariant>& out_data)
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

void krnl_window::反序列化控件附加属性(unsigned char* pUnitDataPtr, QHash<QString, QVariant>& out_data)
{
	WinFrom_UnitData tmpData;
	tmpData.version = CDR_ReadUInt(pUnitDataPtr);

	if (tmpData.version <= 6) {
		tmpData.边框 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.底图方式 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.底色 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.最大化按钮 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.最小化按钮 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.控制按钮 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.位置 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.可否移动 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.播放次数 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.回车下移焦点 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.Esc键关闭 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.F1键打开帮助 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.帮助标志值 = CDR_ReadUInt(pUnitDataPtr);

		if (tmpData.version >= 2) {
			tmpData.在任务条中显示 = CDR_ReadUInt(pUnitDataPtr);
		}
		if (tmpData.version >= 3) {
			tmpData.随意移动 = CDR_ReadUInt(pUnitDataPtr);
			tmpData.外形 = CDR_ReadUInt(pUnitDataPtr);
		}
		if (tmpData.version >= 4) {
			tmpData.总在最前 = CDR_ReadUInt(pUnitDataPtr);
		}
		if (tmpData.version >= 5) {
			tmpData.保持标题条激活 = CDR_ReadUInt(pUnitDataPtr);
		}
		if (tmpData.version == 6) {
			tmpData.窗口类名 = CDR_ReadCString(pUnitDataPtr);
		}

		tmpData.底图 = CDR_ReadCFreqMem(pUnitDataPtr);
		tmpData.图标 = CDR_ReadCFreqMem(pUnitDataPtr);
		tmpData.背景音乐 = CDR_ReadCFreqMem(pUnitDataPtr);

		tmpData.标题 = CDR_ReadCString(pUnitDataPtr);
		tmpData.帮助文件名 = CDR_ReadCString(pUnitDataPtr);
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
	out_data[QStringLiteral("帮助标志值")] = tmpData.帮助标志值;
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

void krnl_window::显示控件属性信息(QHash<QString, QVariant>& map_ControlData)
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
	EAppControl::添加文本控件(QStringLiteral("标题"), map_ControlData[QStringLiteral("标题")].toString());
	EAppControl::添加列表控件(QStringLiteral("边框"), 取边框列表(), map_ControlData[QStringLiteral("边框")].toUInt());
	EAppControl::添加底色控件(QStringLiteral("底色"), map_ControlData[QStringLiteral("底色")].toUInt());
	EAppControl::添加图片控件(QStringLiteral("底图"), map_ControlData[QStringLiteral("底图")].toByteArray());
	EAppControl::添加列表控件(QStringLiteral("    底图方式"), 取底图方式列表(), map_ControlData[QStringLiteral("底图方式")].toUInt());
	krnl_window::添加背景音乐控件(map_ControlData[QStringLiteral("背景音乐")].toByteArray());
	EAppControl::添加列表控件(QStringLiteral("    播放次数"), 取播放次数列表(), map_ControlData[QStringLiteral("播放次数")].toUInt());

	EAppControl::添加布尔控件(QStringLiteral("控制按钮"), map_ControlData[QStringLiteral("控制按钮")].toBool());
	EAppControl::添加布尔控件(QStringLiteral("    最大化按钮"), map_ControlData[QStringLiteral("最大化按钮")].toBool());
	EAppControl::添加布尔控件(QStringLiteral("    最小化按钮"), map_ControlData[QStringLiteral("最小化按钮")].toBool());

	EAppControl::添加列表控件(QStringLiteral("位置"), 取位置列表(), map_ControlData[QStringLiteral("位置")].toUInt());
	EAppControl::添加布尔控件(QStringLiteral("可否移动"), map_ControlData[QStringLiteral("可否移动")].toBool());
	EAppControl::添加图片控件(QStringLiteral("图标"), map_ControlData[QStringLiteral("图标")].toByteArray());
	EAppControl::添加布尔控件(QStringLiteral("回车下移焦点"), map_ControlData[QStringLiteral("回车下移焦点")].toBool());
	EAppControl::添加布尔控件(QStringLiteral("Esc键关闭"), map_ControlData[QStringLiteral("Esc键关闭")].toBool());
	EAppControl::添加布尔控件(QStringLiteral("F1键打开帮助"), map_ControlData[QStringLiteral("F1键打开帮助")].toBool());
	EAppControl::添加文本控件(QStringLiteral("    帮助文件名"), map_ControlData[QStringLiteral("帮助文件名")].toString());
	EAppControl::添加文本控件(QStringLiteral("    帮助标志值"), map_ControlData[QStringLiteral("帮助标志值")].toString());
	EAppControl::添加布尔控件(QStringLiteral("在任务条中显示"), map_ControlData[QStringLiteral("在任务条中显示")].toBool());
	EAppControl::添加布尔控件(QStringLiteral("随意移动"), map_ControlData[QStringLiteral("随意移动")].toBool());
	EAppControl::添加列表控件(QStringLiteral("外形"), 取外形列表(), map_ControlData[QStringLiteral("外形")].toUInt());
	EAppControl::添加布尔控件(QStringLiteral("总在最前"), map_ControlData[QStringLiteral("总在最前")].toBool());
	EAppControl::添加布尔控件(QStringLiteral("保持标题条激活"), map_ControlData[QStringLiteral("保持标题条激活")].toBool());
	EAppControl::添加文本控件(QStringLiteral("窗口类名"), map_ControlData[QStringLiteral("窗口类名")].toString());
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