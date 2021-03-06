#include "krnl_ListBox.h"
#include "../common/public.h"

struct ListBox_UnitData
{
	unsigned int version;
	unsigned int 边框;
	unsigned int 自动排序;
	unsigned int 多列;
	unsigned int 行间距;
	unsigned int 文本颜色;
	unsigned int 背景颜色;
	unsigned int 允许选择多项;
	int 现行选中项;
	qvector<unsigned char> 字体;
	qvector<unsigned char> 项目数值;
	qvector<qstring> 列表项目;
	qstring 数据源;
	qstring 数据列;
	unsigned int Reserved;
};

QStringList krnl_ListBox::取边框列表()
{
	QStringList Items = {
	QStringLiteral("无边框"),
	QStringLiteral("凹入式"),
	QStringLiteral("凸出式"),
	QStringLiteral("浅凹入式"),
	QStringLiteral("镜框式"),
	QStringLiteral("单线边框式"),
	};
	return Items;
}

void krnl_ListBox::取控件默认附加属性(QHash<QString, QVariant>& out_data)
{
	out_data[QStringLiteral("边框")] = 0x1;
	out_data[QStringLiteral("自动排序")] = 0x0;
	out_data[QStringLiteral("多列")] = 0x0;
	out_data[QStringLiteral("行间距")] = 0x1;
	out_data[QStringLiteral("文本颜色")] = 0x0;
	out_data[QStringLiteral("背景颜色")] = 0xFFFFFF;
	out_data[QStringLiteral("允许选择多项")] = 0x0;
	out_data[QStringLiteral("现行选中项")] = -1;
}

void krnl_ListBox::反序列化控件附加属性(unsigned char* pUnitDataPtr, QHash<QString, QVariant>& out_data)
{
	ListBox_UnitData tmpData;
	tmpData.version = CDR_ReadUInt(pUnitDataPtr);
	if (tmpData.version > 3) {
		return;
	}
	tmpData.边框 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.自动排序 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.多列 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.行间距 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.文本颜色 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.背景颜色 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.允许选择多项 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.现行选中项 = CDR_ReadInt(pUnitDataPtr);
	tmpData.字体 = CDR_ReadCFreqMem(pUnitDataPtr);

	QStringList list_项目数值;
	qvector<unsigned char> ret;
	int len = CDR_ReadInt(pUnitDataPtr);
	for (int n = 0; n < len / 4; ++n) {
		list_项目数值.push_back(QString::number(CDR_ReadUInt(pUnitDataPtr)));
	}

	tmpData.列表项目 = CDR_ReadVecString(pUnitDataPtr);
	if (tmpData.version >= 2) {
		tmpData.数据源 = CDR_ReadCString(pUnitDataPtr);
		tmpData.数据列 = CDR_ReadCString(pUnitDataPtr);
	}
	if (tmpData.version >= 3) {
		tmpData.Reserved = CDR_ReadUInt(pUnitDataPtr);
	}
	out_data[QStringLiteral("version")] = tmpData.version;
	out_data[QStringLiteral("边框")] = tmpData.边框;
	out_data[QStringLiteral("自动排序")] = tmpData.自动排序;
	out_data[QStringLiteral("多列")] = tmpData.多列;
	out_data[QStringLiteral("行间距")] = tmpData.行间距;
	out_data[QStringLiteral("文本颜色")] = tmpData.文本颜色;
	out_data[QStringLiteral("背景颜色")] = tmpData.背景颜色;
	out_data[QStringLiteral("允许选择多项")] = tmpData.允许选择多项;
	out_data[QStringLiteral("现行选中项")] = tmpData.现行选中项;
	if (tmpData.字体.size()) {
		out_data[QStringLiteral("字体")] = QByteArray((char*)&tmpData.字体[0], tmpData.字体.size());
	}
	out_data[QStringLiteral("项目数值")] = list_项目数值;
	QStringList list_列表项目;
	for (unsigned int n = 0; n < tmpData.列表项目.size(); ++n) {
		list_列表项目.push_back(QString::fromLocal8Bit(tmpData.列表项目[n].c_str()));
	}
	out_data[QStringLiteral("列表项目")] = list_列表项目;
	out_data[QStringLiteral("数据源")] = QString::fromLocal8Bit(tmpData.数据源.c_str());
	out_data[QStringLiteral("数据列")] = QString::fromLocal8Bit(tmpData.数据列.c_str());
}

void krnl_ListBox::显示控件属性信息(QHash<QString, QVariant>& map_ControlData)
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
	EAppControl::添加列表控件(QStringLiteral("边框"), krnl_ListBox::取边框列表(), map_ControlData[QStringLiteral("边框")].toUInt());
	EAppControl::添加布尔控件(QStringLiteral("自动排序"), map_ControlData[QStringLiteral("自动排序")].toBool());
	EAppControl::添加布尔控件(QStringLiteral("多列"), map_ControlData[QStringLiteral("多列")].toBool());
	EAppControl::添加文本控件(QStringLiteral("行间距"), map_ControlData[QStringLiteral("行间距")].toString());
	EAppControl::添加颜色控件(QStringLiteral("文本颜色"), map_ControlData[QStringLiteral("文本颜色")].toUInt());
	EAppControl::添加颜色控件(QStringLiteral("背景颜色"), map_ControlData[QStringLiteral("背景颜色")].toUInt());
	EAppControl::添加字体控件(QStringLiteral("字体"), map_ControlData[QStringLiteral("字体")].toByteArray());
	EAppControl::添加布尔控件(QStringLiteral("允许选择多项"), map_ControlData[QStringLiteral("允许选择多项")].toBool());
	EAppControl::添加文本控件(QStringLiteral("现行选中项"), map_ControlData[QStringLiteral("现行选中项")].toString());
	EAppControl::添加列表控件(QStringLiteral("列表项目"), map_ControlData[QStringLiteral("列表项目")].toStringList(), 0);
	EAppControl::添加列表控件(QStringLiteral("项目数值"), map_ControlData[QStringLiteral("项目数值")].toStringList(), 0);
	EAppControl::添加文本控件(QStringLiteral("数据源"), map_ControlData[QStringLiteral("数据源")].toString());
	EAppControl::添加文本控件(QStringLiteral("数据列"), map_ControlData[QStringLiteral("数据列")].toString());
}

qstring krnl_ListBox::取事件名称(int eventIndex)
{
	qstring ret;
	switch (eventIndex)
	{
	case 0:
		ret = "列表框被选择";
		break;
	case 1:
		ret = "双击选择";
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

QString krnl_ListBox::取图标资源路径()
{
	return QStringLiteral(":/ICO/ICO/列表框.ico");
}