#include "EAppControlFactory.h"
#include "krnl_window.h"
#include "krnl_EditBox.h"
#include "krnl_Label.h"
#include "krnl_Button.h"

class krnl_window gkrnl_window;
class krnl_EditBox gkrnl_EditBox;
class krnl_Label gkrnl_Label;
class krnl_Button gkrnl_Button;

QHash<ControlType_t, EAppControl*> g_classMap = {
	{krnl_window, &gkrnl_window},
	{krnl_EditBox,&gkrnl_EditBox},
	{krnl_Label,&gkrnl_Label},
	{krnl_Button,&gkrnl_Button},
};

EAppControl* EAppControlFactory::getEAppControl(ControlType_t type)
{
	EAppControl* ret = nullptr;
	return g_classMap[type];
}