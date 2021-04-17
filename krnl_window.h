#pragma once
#include <pro.h>
#include <QHash>
#include <QVariant>

class ControlInfoWidget;
class krnl_window
{
public:
	static qstring 取事件名称(int eventIndex);
	static void 取控件属性(ea_t propertyAddr, int propertySize, QHash<QString, QVariant>* out_Data);
	static void 显示控件信息(unsigned int propertyAddr, int propertySize);
private:
	static void 取默认属性(QHash<QString, QVariant>& out_data);
	static void 反序列化属性(unsigned char* lpControlData, QHash<QString, QVariant>& out_data);
	static void 添加边框控件(ControlInfoWidget* pWindow, unsigned int ProperyValue);
	static void 添加底色控件(ControlInfoWidget* pWindow, unsigned int ProperyValue);
};


qstring 取窗口属性_外形(unsigned int index);

qstring 取窗口属性_位置(unsigned int index);

qstring 取窗口属性_鼠标指针(unsigned int index);

qstring 取窗口属性_边框(unsigned int index);

qstring 取窗口属性_底图方式(unsigned int index);

qstring 取窗口属性_播放次数(unsigned int index);

qstring 取窗口事件名称(int eventIndex);