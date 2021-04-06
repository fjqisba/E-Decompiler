#pragma once
#include <pro.h>

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
	unsigned int field_34;
	unsigned int field_38;
	unsigned int field_3C;
	unsigned int field_40;
	unsigned int 总在最前;
	unsigned int 保持标题条激活;
	qstring 窗口类名;
	qvector<unsigned char> 底图;
	qvector<unsigned char> 图标;
	qvector<unsigned char> 背景音乐;
	qstring 标题;
	qstring 帮助文件名;
};

void Unserialize_WinFormData(unsigned char* lpControlData, WinFrom_UnitData* out_Data);