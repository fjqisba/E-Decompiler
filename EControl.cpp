#include "EControl.h"
#include <QMap>
#include "krnl_window.h"
#include "krnl_Button.h"
#include "krnl_Timer.h"
#include "krnl_ListBox.h"
#include "krnl_RadioBox.h"
#include "krnl_PicBtn.h"
#include "krnl_EditBox.h"
#include "krnl_PicBox.h"
#include "krnl_ShapeBox.h"
#include "krnl_CheckBox.h"
#include "krnl_DropTarget.h"

qstring EControl::取事件名称(ControlType_t type,int eventIndex)
{
	qstring ret;

	static QMap<ControlType_t, qstring(*)(int)> map_eventFunc;
	if (!map_eventFunc.size()) {
		map_eventFunc[krnl_window] = krnl_window::取事件名称;
		map_eventFunc[krnl_EditBox] = krnl_EditBox::取事件名称;
		map_eventFunc[krnl_PicBox] = krnl_PicBox::取事件名称;
		map_eventFunc[krnl_ShapeBox] = krnl_ShapeBox::取事件名称;
		map_eventFunc[krnl_CheckBox] = krnl_CheckBox::取事件名称;
		map_eventFunc[krnl_Button] = krnl_Button::取事件名称;
		map_eventFunc[krnl_Timer] = krnl_Timer::取事件名称;
		map_eventFunc[krnl_ListBox] = krnl_ListBox::取事件名称;
		map_eventFunc[krnl_RadioBox] = krnl_RadioBox::取事件名称;
		map_eventFunc[krnl_PicBtn] = krnl_PicBtn::取事件名称;
		

		map_eventFunc[krnl_DropTarget] = krnl_DropTarget::取事件名称;
 	}

	QMap<ControlType_t, qstring(*)(int)>::iterator it = map_eventFunc.find(type);
	if (it != map_eventFunc.end()) {
		ret = it.value()(eventIndex);
	}
	return ret;
}