#include "krnl_EditBox.h"
#include "../common/public.h"

struct EditBox_UnitData
{
	unsigned int version;
	unsigned int 边框;
	unsigned int 文本颜色;
	unsigned int 背景颜色;
	unsigned int 隐藏选择;
	unsigned int 最大允许长度;
	unsigned int 是否允许多行;
	unsigned int 滚动条;
	unsigned int 对齐方式;
	unsigned int 输入方式;
	char 密码遮盖字符;
	unsigned int 转换方式;
	unsigned int 调节器方式;
	unsigned int 调节器底限值;
	unsigned int 调节器上限值;
	unsigned int 起始选择位置;
	unsigned int 被选择字符数;
	qvector<unsigned char> 字体;
	qstring 内容;
	qstring 数据源;
	qstring 数据列;
};

QStringList krnl_EditBox::取边框列表()
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

QStringList krnl_EditBox::取滚动条列表()
{
	QStringList Items = {
	QStringLiteral("无"),
	QStringLiteral("横向滚动条"),
	QStringLiteral("纵向滚动条"),
	QStringLiteral("横向及纵向滚动条"),
	};
	return Items;
}

QStringList krnl_EditBox::取对齐方式列表()
{
	QStringList Items = {
	QStringLiteral("左对齐"),
	QStringLiteral("居中"),
	QStringLiteral("右对齐"),
	};
	return Items;
}

QStringList krnl_EditBox::取输入方式列表()
{
	QStringList Items = {
	QStringLiteral("通常方式"),
	QStringLiteral("只读方式"),
	QStringLiteral("密码输入"),
	QStringLiteral("整数文本输入"),
	QStringLiteral("小数文本输入"),
	QStringLiteral("输入字节"),
	QStringLiteral("输入短整数"),
	QStringLiteral("输入整数"),
	QStringLiteral("输入长整数"),
	QStringLiteral("输入小数"),
	QStringLiteral("输入双精度小数"),
	QStringLiteral("输入日期时间 "),
	};
	return Items;
}

QStringList krnl_EditBox::取转换方式列表()
{
	QStringList Items = {
	QStringLiteral("无"),
	QStringLiteral("大写->小写"),
	QStringLiteral("小写->大写"),
	};
	return Items;
}

QStringList krnl_EditBox::取调节器方式列表()
{
	QStringList Items = {
	QStringLiteral("无调节器"),
	QStringLiteral("自动调节器"),
	QStringLiteral("手动调节器"),
	};
	return Items;
}

void krnl_EditBox::取控件默认附加属性(QHash<QString, QVariant>& out_data)
{
	out_data[QStringLiteral("边框")] = 1;
	out_data[QStringLiteral("文本颜色")] = 0x0;
	out_data[QStringLiteral("背景颜色")] = 0xFFFFFF;
	out_data[QStringLiteral("隐藏选择")] = 0x1;
	out_data[QStringLiteral("最大允许长度")] = 0;
	out_data[QStringLiteral("是否允许多行")] = 0;
	out_data[QStringLiteral("滚动条")] = 0x0;
	out_data[QStringLiteral("对齐方式")] = 0x0;
	out_data[QStringLiteral("输入方式")] = 0x0;
	out_data[QStringLiteral("密码遮盖字符")] = QString('*');
	out_data[QStringLiteral("转换方式")] = 0x0;
	out_data[QStringLiteral("调节器方式")] = 0x0;
	out_data[QStringLiteral("调节器底限值")] = 0x0;
	out_data[QStringLiteral("调节器上限值")] = 0x64;
	out_data[QStringLiteral("起始选择位置")] = 0x0;
	out_data[QStringLiteral("被选择字符数")] = 0x0;
}

void krnl_EditBox::反序列化控件附加属性(unsigned char* pUnitDataPtr, QHash<QString, QVariant>& out_data)
{
	EditBox_UnitData tmpData;

	tmpData.version = CDR_ReadUInt(pUnitDataPtr);
	if (tmpData.version > 2) {
		return;
	}

	tmpData.边框 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.文本颜色 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.背景颜色 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.隐藏选择 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.最大允许长度 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.是否允许多行 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.滚动条 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.对齐方式 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.输入方式 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.密码遮盖字符 = CDR_ReadChar(pUnitDataPtr);
	tmpData.转换方式 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.调节器方式 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.调节器底限值 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.调节器上限值 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.起始选择位置 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.被选择字符数 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.字体 = CDR_ReadCFreqMem(pUnitDataPtr);
	tmpData.内容 = CDR_ReadCString(pUnitDataPtr);
	if (tmpData.version == 2) {
		tmpData.数据源 = CDR_ReadCString(pUnitDataPtr);
		tmpData.数据列 = CDR_ReadCString(pUnitDataPtr);
	}
	
	//————————————————————————
	out_data[QStringLiteral("version")] = tmpData.version;
	out_data[QStringLiteral("边框")] = tmpData.边框;
	out_data[QStringLiteral("文本颜色")] = tmpData.文本颜色;
	out_data[QStringLiteral("背景颜色")] = tmpData.背景颜色;

	out_data[QStringLiteral("隐藏选择")] = tmpData.隐藏选择;
	out_data[QStringLiteral("最大允许长度")] = tmpData.最大允许长度;
	out_data[QStringLiteral("是否允许多行")] = tmpData.是否允许多行;
	out_data[QStringLiteral("滚动条")] = tmpData.滚动条;
	out_data[QStringLiteral("对齐方式")] = tmpData.对齐方式;
	out_data[QStringLiteral("输入方式")] = tmpData.输入方式;
	out_data[QStringLiteral("密码遮盖字符")] = QString(tmpData.密码遮盖字符);
	out_data[QStringLiteral("转换方式")] = tmpData.转换方式;
	out_data[QStringLiteral("调节器方式")] = tmpData.调节器方式;
	out_data[QStringLiteral("调节器底限值")] = tmpData.调节器底限值;
	out_data[QStringLiteral("调节器上限值")] = tmpData.调节器上限值;
	out_data[QStringLiteral("起始选择位置")] = tmpData.起始选择位置;
	out_data[QStringLiteral("被选择字符数")] = tmpData.被选择字符数;
	if (tmpData.字体.size()) {
		out_data[QStringLiteral("字体")] = QByteArray((char*)&tmpData.字体[0], tmpData.字体.size());
	}
	out_data[QStringLiteral("内容")] = QString::fromLocal8Bit(tmpData.内容.c_str());
	out_data[QStringLiteral("数据源")] = QString::fromLocal8Bit(tmpData.数据源.c_str());
	out_data[QStringLiteral("数据列")] = QString::fromLocal8Bit(tmpData.数据列.c_str());
	return;
}

void krnl_EditBox::显示控件属性信息(QHash<QString, QVariant>& map_ControlData)
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
	EAppControl::添加文本控件(QStringLiteral("内容"), map_ControlData[QStringLiteral("内容")].toString());
	EAppControl::添加列表控件(QStringLiteral("边框"), krnl_EditBox::取边框列表(), map_ControlData[QStringLiteral("边框")].toUInt());
	EAppControl::添加颜色控件(QStringLiteral("文本颜色"), map_ControlData[QStringLiteral("文本颜色")].toUInt());
	EAppControl::添加颜色控件(QStringLiteral("背景颜色"), map_ControlData[QStringLiteral("背景颜色")].toUInt());
	EAppControl::添加字体控件(QStringLiteral("字体"), map_ControlData[QStringLiteral("字体")].toByteArray());
	EAppControl::添加布尔控件(QStringLiteral("隐藏选择"), map_ControlData[QStringLiteral("隐藏选择")].toBool());
	EAppControl::添加文本控件(QStringLiteral("最大允许长度"), map_ControlData[QStringLiteral("最大允许长度")].toString());
	EAppControl::添加布尔控件(QStringLiteral("是否允许多行"), map_ControlData[QStringLiteral("是否允许多行")].toBool());
	EAppControl::添加列表控件(QStringLiteral("滚动条"), krnl_EditBox::取滚动条列表(), map_ControlData[QStringLiteral("滚动条")].toUInt());
	EAppControl::添加列表控件(QStringLiteral("对齐方式"), krnl_EditBox::取对齐方式列表(), map_ControlData[QStringLiteral("对齐方式")].toUInt());
	EAppControl::添加列表控件(QStringLiteral("输入方式"), krnl_EditBox::取输入方式列表(), map_ControlData[QStringLiteral("输入方式")].toUInt());
	EAppControl::添加文本控件(QStringLiteral("    密码遮盖字符"), map_ControlData[QStringLiteral("密码遮盖字符")].toString());
	EAppControl::添加列表控件(QStringLiteral("转换方式"), krnl_EditBox::取转换方式列表(), map_ControlData[QStringLiteral("转换方式")].toUInt());
	EAppControl::添加列表控件(QStringLiteral("调节器方式"), krnl_EditBox::取调节器方式列表(), map_ControlData[QStringLiteral("调节器方式")].toUInt());
	EAppControl::添加文本控件(QStringLiteral("    调节器底限值"), map_ControlData[QStringLiteral("调节器底限值")].toString());
	EAppControl::添加文本控件(QStringLiteral("    调节器上限值"), map_ControlData[QStringLiteral("调节器上限值")].toString());
	EAppControl::添加文本控件(QStringLiteral("起始选择位置"), map_ControlData[QStringLiteral("起始选择位置")].toString());
	EAppControl::添加文本控件(QStringLiteral("被选择字符数"), map_ControlData[QStringLiteral("被选择字符数")].toString());
	EAppControl::添加无效控件(QStringLiteral("被选择文本"), QStringLiteral("** 设计时不可用 **"));
	EAppControl::添加文本控件(QStringLiteral("数据源"), map_ControlData[QStringLiteral("数据源")].toString());
	EAppControl::添加文本控件(QStringLiteral("数据列"), map_ControlData[QStringLiteral("数据列")].toString());
	return;
}

QString krnl_EditBox::取图标资源路径()
{
	return QStringLiteral(":/ICO/ICO/编辑框.ico");
}

qstring krnl_EditBox::取事件名称(int eventIndex)
{
	qstring ret;
	switch (eventIndex)
	{
	case 0:
		ret = "内容被改变";
		break;
	case 1:
		ret = "调节钮被按下";
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