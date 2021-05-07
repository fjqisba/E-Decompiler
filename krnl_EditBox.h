#pragma once
#include <pro.h>
#include "EAppControl.h"

class krnl_EditBox :public EAppControl
{
protected:
	void 取控件默认附加属性(QHash<QString, QVariant>& out_data);
	void 反序列化控件附加属性(unsigned char* pUnitDataPtr, QHash<QString, QVariant>& out_data);
	void 显示控件属性信息(QHash<QString, QVariant>& out_data);
	qstring 取事件名称(int eventIndex);
	QString 取图标资源路径();
private:
	QStringList 取边框列表();
	QStringList 取滚动条列表();
	QStringList 取对齐方式列表();
	QStringList 取输入方式列表();
	QStringList 取转换方式列表();
	QStringList 取调节器方式列表();
};