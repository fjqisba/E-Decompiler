#include "EAppControlFactory.h"
#include "../ESymbol.h"
#include "../Utils/Common.h"
#include "CKrnl_window.h"
#include "CKrnl_EditBox.h"
#include "CKrnl_PicBox.h"
#include "CKrnl_ShapeBox.h"
#include "CKrnl_DrawPanel.h"
#include "CKrnl_Label.h"
#include "CKrnl_Button.h"
#include "CKrnl_GroupBox.h"
#include "CKrnl_CheckBox.h"
#include "CKrnl_ChkListBox.h"
#include "CKrnl_AnimateBox.h"
#include "CKrnl_RadioBox.h"
#include "CKrnl_ComboBox.h"
#include "CKrnl_ListBox.h"
#include "CKrnl_HScrollBar.h"
#include "CKrnl_VScrollBar.h"
#include "CKrnl_ProcessBar.h"
#include "CKrnl_SliderBar.h"
#include "CKrnl_Tab.h"
#include "CKrnl_Timer.h"

EAppControlFactory& EAppControlFactory::Instance()
{
	static EAppControlFactory instance;
	return instance;
}


void EAppControlFactory::RegisterEAppControl(std::string eControlLibInfo,unsigned int controlTypeId)
{
	//库Guid + 控件名称 => 控件类型
	static std::map<std::string, fCreateEAppControl> tmpControlMap;
	if (!tmpControlMap.size()) {
		tmpControlMap["d09f2340818511d396f6aaf844c7e325窗口"] = fCreateEAppControl(CKrnl_window::create);
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325菜单"] = fCreateEAppControl(CKrnl_Menu::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325编辑框"] = fCreateEAppControl(CKrnl_EditBox::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325图片框"] = fCreateEAppControl(CKrnl_PicBox::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325外形框"] = fCreateEAppControl(CKrnl_ShapeBox::create);;
		tmpControlMap["d09f2340818511d396f6aaf844c7e325画板"] = fCreateEAppControl(CKrnl_DrawPanel::create);;
		tmpControlMap["d09f2340818511d396f6aaf844c7e325分组框"] = fCreateEAppControl(CKrnl_GroupBox::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325标签"] = fCreateEAppControl(CKrnl_Label::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325按钮"] = fCreateEAppControl(CKrnl_Button::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325选择框"] = fCreateEAppControl(CKrnl_CheckBox::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325单选框"] = fCreateEAppControl(CKrnl_RadioBox::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325组合框"] = fCreateEAppControl(CKrnl_ComboBox::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325列表框"] = fCreateEAppControl(CKrnl_ListBox::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325选择列表框"] = fCreateEAppControl(CKrnl_ChkListBox::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325横向滚动条"] = fCreateEAppControl(CKrnl_HScrollBar::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325纵向滚动条"] = fCreateEAppControl(CKrnl_VScrollBar::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325进度条"] = fCreateEAppControl(CKrnl_ProcessBar::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325滑块条"] = fCreateEAppControl(CKrnl_SliderBar::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325选择夹"] = fCreateEAppControl(CKrnl_Tab::create);
		tmpControlMap["d09f2340818511d396f6aaf844c7e325影像框"] = fCreateEAppControl(CKrnl_AnimateBox::create);
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325日期框"] = krnl_DatePicker;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325月历"] = krnl_MonthCalendar;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325驱动器框"] = krnl_DriverBox;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325目录框"] = krnl_DirBox;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325文件框"] = krnl_FileBox;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325颜色选择器"] = krnl_ColorPicker;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325超级链接器"] = krnl_HyperLinker;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325调节器"] = krnl_Spin;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325通用对话框"] = krnl_CommonDlg;
		tmpControlMap["d09f2340818511d396f6aaf844c7e325时钟"] = fCreateEAppControl(CKrnl_Timer::create);
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325打印机"] = krnl_printer;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325数据报"] = krnl_UDP;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325客户"] = krnl_Client;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325服务器"] = krnl_Server;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325端口"] = krnl_SerialPort;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325表格"] = krnl_Grid;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325数据源"] = krnl_DataSrc;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325通用提供者"] = krnl_NProvider;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325数据库提供者"] = krnl_DBProvider;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325图形按钮"] = krnl_PicBtn;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325外部数据库"] = krnl_ODBCDB;
		//tmpControlMap["d09f2340818511d396f6aaf844c7e325外部数据提供者"] = krnl_ODBCProvider;
		//tmpControlMap["{9DA96BF9CEBD45c5BFCF94CBE61671F5}拖放对象"] = krnl_DropTarget;
	}

	std::map<std::string, fCreateEAppControl>::iterator it = tmpControlMap.find(eControlLibInfo);
	if (it != tmpControlMap.end()) {
		registerMap[controlTypeId] = it->second;
	}
	else {
		registerMap[controlTypeId] = fCreateEAppControl(EAppControl_Unknow::create);
	}
}

EAppControl* EAppControlFactory::CreateEAppControl(unsigned int dwControlTypeId)
{
	std::map<unsigned int, fCreateEAppControl>::iterator it = registerMap.find(dwControlTypeId);
	if (it == registerMap.end()) {
		return nullptr;
	}
	fCreateEAppControl fCreate = it->second;
	return (EAppControl*)fCreate();
}

EAppControl_Unknow* EAppControl_Unknow::create()
{
	return new EAppControl_Unknow();
}

std::string EAppControl_Unknow::GetEventName(int eventIndex)
{
	return "";
}

bool EAppControl_Unknow::InitControlExtraData(unsigned int propertyAddr, unsigned int propertySize)
{
	return true;
}

std::string EAppControl_Unknow::GetPropertyName(unsigned int propertyIndex)
{
	return "未知";
}
