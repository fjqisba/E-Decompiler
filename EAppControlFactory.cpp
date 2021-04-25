#include "EAppControlFactory.h"
#include "krnl_window.h"
#include "krnl_EditBox.h"
#include "krnl_Label.h"
#include "krnl_Button.h"
#include "krnl_Timer.h"
#include "krnl_PicBox.h"

class krnl_window gkrnl_window;
class krnl_EditBox gkrnl_EditBox;
class krnl_PicBox gkrnl_PicBox;
class krnl_Label gkrnl_Label;
class krnl_Button gkrnl_Button;
class krnl_Timer gkrnl_Timer;


QHash<ControlType_t, EAppControl*> g_classMap = {
	{krnl_window, &gkrnl_window},
	{krnl_EditBox,&gkrnl_EditBox},
	{krnl_PicBox,&gkrnl_PicBox},
	{krnl_Label,&gkrnl_Label},
	{krnl_Button,&gkrnl_Button},
	{krnl_Timer,&gkrnl_Timer},
};

EAppControl* EAppControlFactory::getEAppControl(ControlType_t type)
{
	EAppControl* ret = nullptr;
	return g_classMap[type];
}