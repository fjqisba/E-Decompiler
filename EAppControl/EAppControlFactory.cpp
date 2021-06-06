#include "EAppControlFactory.h"
#include "krnl_window.h"
#include "krnl_EditBox.h"
#include "krnl_Label.h"
#include "krnl_Button.h"
#include "krnl_Timer.h"
#include "krnl_PicBox.h"
#include "krnl_ShapeBox.h"
#include "krnl_DrawPanel.h"
#include "krnl_GroupBox.h"
#include "krnl_CheckBox.h"
#include "krnl_RadioBox.h"

class krnl_window gkrnl_window;
class krnl_EditBox gkrnl_EditBox;
class krnl_PicBox gkrnl_PicBox;
class krnl_Label gkrnl_Label;
class krnl_Button gkrnl_Button;
class krnl_Timer gkrnl_Timer;
class krnl_ShapeBox gkrnl_ShapeBox;
class krnl_DrawPanel gkrnl_DrawPanel;
class krnl_GroupBox gkrnl_GroupBox;
class krnl_CheckBox gkrnl_CheckBox;
class krnl_RadioBox gkrnl_RadioBox;

QHash<ControlType_t, EAppControl*> g_classMap = {
	{krnl_window, &gkrnl_window},
	{krnl_EditBox,&gkrnl_EditBox},
	{krnl_PicBox,&gkrnl_PicBox},
	{krnl_Label,&gkrnl_Label},
	{krnl_Button,&gkrnl_Button},
	{krnl_Timer,&gkrnl_Timer},
	{krnl_ShapeBox,&gkrnl_ShapeBox},
	{krnl_DrawPanel,&gkrnl_DrawPanel},
	{krnl_GroupBox,&gkrnl_GroupBox},
	{krnl_CheckBox,&gkrnl_CheckBox},
	{krnl_RadioBox,&gkrnl_RadioBox},
};

EAppControl* EAppControlFactory::getEAppControl(ControlType_t type)
{
	EAppControl* ret = nullptr;
	return g_classMap[type];
}