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

void Unserialize_WinFormData(unsigned char* lpControlData, WinFrom_UnitData* out_Data);

void 取窗口属性_默认(WinFrom_UnitData* out_Data);

qstring 取窗口属性_外形(unsigned int index);

qstring 取窗口属性_位置(unsigned int index);

qstring 取窗口属性_鼠标指针(unsigned int index);

qstring 取窗口属性_边框(unsigned int index);

qstring 取窗口属性_底图方式(unsigned int index);

qstring 取窗口属性_播放次数(unsigned int index);