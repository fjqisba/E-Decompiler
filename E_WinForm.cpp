#include "E_WinForm.h"
#include "public.h"

void Unserialize_WinFormData(unsigned char* lpControlData, WinFrom_UnitData* out_Data)
{
	out_Data->version = CDR_ReadUInt(lpControlData);

	if (out_Data->version <= 6) {
		out_Data->边框 = CDR_ReadUInt(lpControlData);
		out_Data->底图方式 = CDR_ReadUInt(lpControlData);
		out_Data->底色 = CDR_ReadUInt(lpControlData);

		out_Data->最大化按钮 = CDR_ReadUInt(lpControlData);
		out_Data->最小化按钮 = CDR_ReadUInt(lpControlData);
		out_Data->控制按钮 = CDR_ReadUInt(lpControlData);
		out_Data->位置 = CDR_ReadUInt(lpControlData);
		out_Data->可否移动 = CDR_ReadUInt(lpControlData);
		out_Data->播放次数 = CDR_ReadUInt(lpControlData);
		out_Data->回车下移焦点 = CDR_ReadUInt(lpControlData);
		out_Data->Esc键关闭 = CDR_ReadUInt(lpControlData);
		out_Data->F1键打开帮助 = CDR_ReadUInt(lpControlData);
		out_Data->帮助标记值 = CDR_ReadUInt(lpControlData);

		if (out_Data->version >= 2) {
			out_Data->在任务条中显示 = CDR_ReadUInt(lpControlData);
		}
		if (out_Data->version >= 3) {
			out_Data->随意移动 = CDR_ReadUInt(lpControlData);
			out_Data->外形 = CDR_ReadUInt(lpControlData);
		}
		if (out_Data->version >= 4) {
			out_Data->总在最前 = CDR_ReadUInt(lpControlData);
		}
		if (out_Data->version >= 5) {
			out_Data->保持标题条激活 = CDR_ReadUInt(lpControlData);
		}
		if (out_Data->version == 6) {
			out_Data->窗口类名 = CDR_ReadCString(lpControlData);
		}

		out_Data->底图 = CDR_ReadCFreqMem(lpControlData);
		out_Data->图标 = CDR_ReadCFreqMem(lpControlData);
		out_Data->背景音乐 = CDR_ReadCFreqMem(lpControlData);

		out_Data->标题 = CDR_ReadCString(lpControlData);
		out_Data->帮助文件名 = CDR_ReadCString(lpControlData);
	}
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

qstring 取窗口属性_鼠标指针(unsigned int index)
{
	qstring ret;
	switch (index)
	{
	case 0x0:
		ret = "默认型";
		break;
	case 0x00007F00:
		ret = "标准箭头型";
		break;
	case 0x00007F03:
		ret = "十字型";
		break;
	case 0x00007F01:
		ret = "文本编辑型";
		break;
	case 0x00007F02:
		ret = "沙漏型";
		break;
	case 0x00007F8B:
		ret = "箭头及问号型";
		break;
	case 0x00007F8A:
		ret = "箭头及沙漏型";
		break;
	case 0x00007F88:
		ret = "禁止符型";
		break;
	case 0x00007F86:
		ret = "四向箭头型";
		break;
	case 0x00007F83:
		ret = "北<->东箭头型";
		break;
	case 0x00007F85:
		ret = "北<->南箭头型";
		break;
	case 0x00007F82:
		ret = "北<->西箭头型";
		break;
	case 0x00007F84:
		ret = "西<->东箭头型";
		break;
	case 0x00007F04:
		ret = "向上箭头型";
		break;
	case 0x00007F89:
		ret = "手型";
		break;
	case 0xFFFFFFFF:
		ret = "自定义型";
		break;
	default:
		ret = "解析错误";
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

void 取窗口属性_默认(WinFrom_UnitData* out_Data)
{
	out_Data->version = 6;
	out_Data->边框 = 2;
	out_Data->底色 = 0xFF000000;
	out_Data->底图方式 = 1;
	out_Data->播放次数 = 0;
	out_Data->控制按钮 = 1;
	out_Data->最大化按钮 = 0;
	out_Data->最小化按钮 = 1;
	out_Data->位置 = 1;
	out_Data->可否移动 = 1;
	out_Data->回车下移焦点 = 0;
	out_Data->Esc键关闭 = 1;
	out_Data->F1键打开帮助 = 0;
	out_Data->在任务条中显示 = 1;
	out_Data->随意移动 = 0;
	out_Data->外形 = 0;
	out_Data->总在最前 = 0;
	out_Data->保持标题条激活 = 0;
}

qstring 取窗口事件名称(int eventIndex)
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