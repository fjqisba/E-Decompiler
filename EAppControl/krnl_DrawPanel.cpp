#include "krnl_DrawPanel.h"
#include "../common/public.h"

struct DrawPanel_UnitData
{
	unsigned int version;
	unsigned int 边框;
	unsigned int 画板背景色;
	unsigned int 底图方式;
	unsigned int 自动重画;
	unsigned int 绘画单位;
	unsigned int 画笔类型;
	unsigned int 画出方式;
	unsigned int 画笔粗细;
	unsigned int 画笔颜色;
	unsigned int 刷子类型;
	unsigned int 刷子颜色;
	unsigned int 文本颜色;
	unsigned int 文本背景颜色;
	qvector<unsigned char> 字体;
	qvector<unsigned char> 底图;
};

QStringList krnl_DrawPanel::取边框列表()
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

QStringList krnl_DrawPanel::取底图方式列表()
{
	QStringList Items = {
	QStringLiteral("图片居左上"),
	QStringLiteral("图片平铺"),
	QStringLiteral("图片居中"),
	};
	return Items;
}

QStringList krnl_DrawPanel::取绘画单位列表()
{
	QStringList Items = {
	QStringLiteral("像素点"),
	QStringLiteral("0.1毫米"),
	QStringLiteral("0.01毫米"),
	QStringLiteral("0.01英寸"),
	QStringLiteral("0.001英寸"),
	QStringLiteral("1/1440英寸"),
	};
	return Items;
}

QStringList krnl_DrawPanel::取画笔类型列表()
{
	QStringList Items = {
	QStringLiteral("空笔"),
	QStringLiteral("直线"),
	QStringLiteral("划线"),
	QStringLiteral("点线"),
	QStringLiteral("点划线"),
	QStringLiteral("双点划线"),
	QStringLiteral("内直线"),
	};
	return Items;
}

QStringList krnl_DrawPanel::取画出方式列表()
{
	QStringList Items = {
		QStringLiteral("黑色"),
		QStringLiteral("非或笔"),
		QStringLiteral("与非笔"),
		QStringLiteral("非复制笔"),
		QStringLiteral("与笔非"),
		QStringLiteral("反转"),
		QStringLiteral("异或笔"),
		QStringLiteral("非与笔"),
		QStringLiteral("与笔"),
		QStringLiteral("非异或笔"),
		QStringLiteral("无操作"),
		QStringLiteral("或非笔"),
		QStringLiteral("复制笔"),
		QStringLiteral("或笔非"),
		QStringLiteral("或笔"),
		QStringLiteral("白色"),
	};
	return Items;
}

QStringList krnl_DrawPanel::取刷子类型列表()
{
	QStringList Items = {
		QStringLiteral("空刷子"),//0
		QStringLiteral("颜色刷子"),//1
		QStringLiteral("右斜线"),//2
		QStringLiteral("直交叉线"),//3
		QStringLiteral("斜交叉线"),//4
		QStringLiteral("左斜线"),//5
		QStringLiteral("横线"),//6
		QStringLiteral("竖线"),//7
		QStringLiteral("5%色点"),//8
		QStringLiteral("10%色点"),//9
		QStringLiteral("20%色点"),//10
		QStringLiteral("25%色点"),//11
		QStringLiteral("30%色点"),//12
		QStringLiteral("40%色点"),//13
		QStringLiteral("50%色点"),//14
		QStringLiteral("60%色点"),//15
		QStringLiteral("70%色点"),//16
		QStringLiteral("75%色点"),//17
		QStringLiteral("80%色点"),//18
		QStringLiteral("90%色点"),//19
		QStringLiteral("浅色上对角线"),//20
		QStringLiteral("浅色下对角线"),//21
		QStringLiteral("深色上对角线"),//22
		QStringLiteral("深色下对角线"),//23
		QStringLiteral("宽上对角线"),//24
		QStringLiteral("宽下对角线"),//25
		QStringLiteral("浅色竖线"),//26
		QStringLiteral("浅色横线"),//27
		QStringLiteral("窄竖线"),//28
		QStringLiteral("窄横线"),//29
		QStringLiteral("深色竖线"),//30
		QStringLiteral("深色横线"),//31
		QStringLiteral("上对角虚线"),//32
		QStringLiteral("下对角虚线"),//33
		QStringLiteral("横虚线"),//34
		QStringLiteral("竖虚线"),//35
		QStringLiteral("小纸屑"),//36
		QStringLiteral("大纸屑"),//37
		QStringLiteral("之字形"),//38
		QStringLiteral("波浪线"),//39
		QStringLiteral("对角砖形"),//40
		QStringLiteral("横向砖形"),//41
		QStringLiteral("纺织物"),//42
		QStringLiteral("方格呢"),//43
		QStringLiteral("草皮"),//44
		QStringLiteral("虚格线"),//45
		QStringLiteral("点式菱形"),//46
		QStringLiteral("瓦形"),//47
		QStringLiteral("棚架"),//48
		QStringLiteral("球体"),//49
		QStringLiteral("小网格"),//50
		QStringLiteral("大网格"),//51
		QStringLiteral("小棋盘"),//52
		QStringLiteral("大棋盘"),//53
		QStringLiteral("空心菱形"),//54
		QStringLiteral("实心菱形"),//55
	};
	return Items;
}

void krnl_DrawPanel::取控件默认附加属性(QHash<QString, QVariant>& out_data)
{
	out_data[QStringLiteral("边框")] = 0x0;
	out_data[QStringLiteral("画板背景色")] = 0xFF000000;
	out_data[QStringLiteral("底图方式")] = 0x1;
	out_data[QStringLiteral("自动重画")] = 0x0;
	out_data[QStringLiteral("绘画单位")] = 0x0;
	out_data[QStringLiteral("画笔类型")] = 0x1;
	out_data[QStringLiteral("画出方式")] = 0xC;
	out_data[QStringLiteral("画笔粗细")] = 0x0;
	out_data[QStringLiteral("画笔颜色")] = 0x0;
	out_data[QStringLiteral("刷子类型")] = 0x1;
	out_data[QStringLiteral("刷子颜色")] = 0xFFFFFF;
	out_data[QStringLiteral("文本颜色")] = 0x0;
	out_data[QStringLiteral("文本背景颜色")] = 0xFF000000;
}

void krnl_DrawPanel::反序列化控件附加属性(unsigned char* pUnitDataPtr, QHash<QString, QVariant>& out_data)
{
	DrawPanel_UnitData tmpData;
	tmpData.version = CDR_ReadUInt(pUnitDataPtr);
	if (tmpData.version > 1) {
		return;
	}

	tmpData.边框 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.画板背景色 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.底图方式 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.自动重画 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.绘画单位 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.画笔类型 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.画出方式 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.画笔粗细 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.画笔颜色 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.刷子类型 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.刷子颜色 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.文本颜色 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.文本背景颜色 = CDR_ReadUInt(pUnitDataPtr);
	tmpData.字体 = CDR_ReadCFreqMem(pUnitDataPtr);
	tmpData.底图 = CDR_ReadCFreqMem(pUnitDataPtr);

	out_data[QStringLiteral("边框")] = tmpData.边框;
	out_data[QStringLiteral("画板背景色")] = tmpData.画板背景色;
	out_data[QStringLiteral("底图方式")] = tmpData.底图方式;
	out_data[QStringLiteral("自动重画")] = tmpData.自动重画;
	out_data[QStringLiteral("绘画单位")] = tmpData.绘画单位;
	out_data[QStringLiteral("画笔类型")] = tmpData.画笔类型;
	out_data[QStringLiteral("画出方式")] = tmpData.画出方式;
	out_data[QStringLiteral("画笔粗细")] = tmpData.画笔粗细;
	out_data[QStringLiteral("画笔颜色")] = tmpData.画笔颜色;
	out_data[QStringLiteral("刷子类型")] = tmpData.刷子类型;
	out_data[QStringLiteral("刷子颜色")] = tmpData.刷子颜色;
	out_data[QStringLiteral("文本颜色")] = tmpData.文本颜色;
	out_data[QStringLiteral("文本颜色")] = tmpData.文本颜色;
	out_data[QStringLiteral("文本背景颜色")] = tmpData.文本背景颜色;
	
	if (tmpData.字体.size()) {
		out_data[QStringLiteral("字体")] = QByteArray((char*)&tmpData.字体[0], tmpData.字体.size());
	}
	if (tmpData.底图.size()) {
		out_data[QStringLiteral("底图")] = QByteArray((char*)&tmpData.底图[0], tmpData.底图.size());
	}
	return;
}

void krnl_DrawPanel::显示控件属性信息(QHash<QString, QVariant>& map_ControlData)
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

	EAppControl::添加列表控件(QStringLiteral("边框"), krnl_DrawPanel::取边框列表(), map_ControlData[QStringLiteral("边框")].toUInt());
	EAppControl::添加颜色控件(QStringLiteral("画板背景色"), map_ControlData[QStringLiteral("画板背景色")].toUInt());
	EAppControl::添加图片控件(QStringLiteral("底图"), map_ControlData[QStringLiteral("底图")].toByteArray());
	EAppControl::添加列表控件(QStringLiteral("    底图方式"), krnl_DrawPanel::取底图方式列表(), map_ControlData[QStringLiteral("底图方式")].toUInt());
	EAppControl::添加布尔控件(QStringLiteral("自动重画"), map_ControlData[QStringLiteral("自动重画")].toBool());
	EAppControl::添加列表控件(QStringLiteral("绘画单位"), krnl_DrawPanel::取绘画单位列表(), map_ControlData[QStringLiteral("绘画单位")].toUInt());
	EAppControl::添加列表控件(QStringLiteral("画笔类型"), krnl_DrawPanel::取画笔类型列表(), map_ControlData[QStringLiteral("画笔类型")].toUInt());
	EAppControl::添加列表控件(QStringLiteral("画出方式"), krnl_DrawPanel::取画出方式列表(), map_ControlData[QStringLiteral("画出方式")].toUInt());
	EAppControl::添加文本控件(QStringLiteral("画笔粗细"), map_ControlData[QStringLiteral("画笔粗细")].toString());
	EAppControl::添加颜色控件(QStringLiteral("画笔颜色"), map_ControlData[QStringLiteral("画笔颜色")].toUInt());
	EAppControl::添加列表控件(QStringLiteral("刷子类型"), krnl_DrawPanel::取刷子类型列表(), map_ControlData[QStringLiteral("刷子类型")].toUInt());
	EAppControl::添加颜色控件(QStringLiteral("刷子颜色"), map_ControlData[QStringLiteral("刷子颜色")].toUInt());
	EAppControl::添加颜色控件(QStringLiteral("文本颜色"), map_ControlData[QStringLiteral("文本颜色")].toUInt());
	EAppControl::添加颜色控件(QStringLiteral("文本背景颜色"), map_ControlData[QStringLiteral("文本背景颜色")].toUInt());
	EAppControl::添加字体控件(QStringLiteral("字体"), map_ControlData[QStringLiteral("字体")].toByteArray());
}

qstring krnl_DrawPanel::取事件名称(int eventIndex)
{
	qstring ret;
	switch (eventIndex)
	{
	case 0:
		ret = "绘画";
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
	default:
		ret = "未知事件";
		break;
	}

	return ret;
}

QString krnl_DrawPanel::取图标资源路径()
{
	return QStringLiteral(":/ICO/ICO/画板.ico");
}