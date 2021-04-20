#pragma once
#include <pro.h>
#include <QHash>
#include <QVariant>
#include "EAppControl.h"

class krnl_Button :public EAppControl
{
public:
	void 取控件默认附加属性(QHash<QString, QVariant>& out_data);
	void 反序列化控件附加属性(unsigned char* pUnitDataPtr, QHash<QString, QVariant>& out_data);
	void 显示控件属性信息(QHash<QString, QVariant>& out_data);
	qstring 取事件名称(int eventIndex);
};


