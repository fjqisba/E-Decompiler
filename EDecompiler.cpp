#include "EDecompiler.h"
#include <bytes.hpp>
#include <segment.hpp>
#include <kernwin.hpp>
#include "IDAMenu.h"
#include "public.h"
#include <ua.hpp>
#include <allins.hpp>
#include <name.hpp>
#include <xref.hpp>
#include "ControlInfoWidget.h"
#include <QString>
#include <QLocale>
#include "UserResourceParser.h"
#include "GuiParser.h"

EDecompilerEngine g_MyDecompiler;

unsigned int GetDataTypeType(unsigned int typeID)
{
	unsigned int result = typeID;
	if (typeID)
	{
		if ((typeID & 0xC0000000) == 0x80000000) {
			result = 1;
		}
		else {
			result = ((typeID & 0xC0000000) != 0x40000000) + 2;
		}
	}
	return result;
}


qstring EDecompilerEngine::GetControlTypeName(uint32 typeId)
{
	qstring ret;
	if (GetDataTypeType(typeId) != 3) {
		return ret;
	}

	int libIndex = (typeId >> 0x10) - 1;
	if (libIndex >= g_MyDecompiler.m_eAppInfo.mVec_LibInfo.size()) {
		return ret;
	}
	int typeIndex = (unsigned short)typeId - 1;
	if (typeIndex >= g_MyDecompiler.m_eAppInfo.mVec_LibInfo[libIndex].mVec_DataTypeInfo.size()) {
		return ret;
	}
	ret = g_MyDecompiler.m_eAppInfo.mVec_LibInfo[libIndex].mVec_DataTypeInfo[typeIndex].m_Name;
	return ret;
}


bool EDecompilerEngine::ParseLibInfomation(ea_t lpLibStartAddr, uint32 dwLibCount)
{
	GuiParser::ClearControlTypeMap();
	
	//库Guid + 控件名称 => 控件类型
	static QMap<qstring, ControlType_t> ControlMap;
	if (!ControlMap.size()) {
		ControlMap["d09f2340818511d396f6aaf844c7e325窗口"] = krnl_window;
		ControlMap["d09f2340818511d396f6aaf844c7e325菜单"] = krnl_menu;
		ControlMap["d09f2340818511d396f6aaf844c7e325编辑框"] = krnl_EditBox;
		ControlMap["d09f2340818511d396f6aaf844c7e325图片框"] = krnl_PicBox;
		ControlMap["d09f2340818511d396f6aaf844c7e325外形框"] = krnl_ShapeBox;
		ControlMap["d09f2340818511d396f6aaf844c7e325画板"] = krnl_DrawPanel;
		ControlMap["d09f2340818511d396f6aaf844c7e325分组框"] = krnl_GroupBox;
		ControlMap["d09f2340818511d396f6aaf844c7e325标签"] = krnl_Label;
		ControlMap["d09f2340818511d396f6aaf844c7e325按钮"] = krnl_Button;
		ControlMap["d09f2340818511d396f6aaf844c7e325选择框"] = krnl_CheckBox;
		ControlMap["d09f2340818511d396f6aaf844c7e325单选框"] = krnl_RadioBox;
		ControlMap["d09f2340818511d396f6aaf844c7e325组合框"] = krnl_ComboBox;
		ControlMap["d09f2340818511d396f6aaf844c7e325列表框"] = krnl_ListBox;
		ControlMap["d09f2340818511d396f6aaf844c7e325选择列表框"] = krnl_ChkListBox;
		ControlMap["d09f2340818511d396f6aaf844c7e325横向滚动条"] = krnl_HScrollBar;
		ControlMap["d09f2340818511d396f6aaf844c7e325纵向滚动条"] = krnl_VScrollBar;
		ControlMap["d09f2340818511d396f6aaf844c7e325进度条"] = krnl_ProcessBar;
		ControlMap["d09f2340818511d396f6aaf844c7e325滑块条"] = krnl_SliderBar;
		ControlMap["d09f2340818511d396f6aaf844c7e325选择夹"] = krnl_Tab;
		ControlMap["d09f2340818511d396f6aaf844c7e325影像框"] = krnl_AnimateBox;
		ControlMap["d09f2340818511d396f6aaf844c7e325日期框"] = krnl_DatePicker;
		ControlMap["d09f2340818511d396f6aaf844c7e325月历"] = krnl_MonthCalendar;
		ControlMap["d09f2340818511d396f6aaf844c7e325驱动器框"] = krnl_DriverBox;
		ControlMap["d09f2340818511d396f6aaf844c7e325目录框"] = krnl_DirBox;
		ControlMap["d09f2340818511d396f6aaf844c7e325文件框"] = krnl_FileBox;
		ControlMap["d09f2340818511d396f6aaf844c7e325颜色选择器"] = krnl_ColorPicker;
		ControlMap["d09f2340818511d396f6aaf844c7e325超级链接器"] = krnl_HyperLinker;
		ControlMap["d09f2340818511d396f6aaf844c7e325调节器"] = krnl_Spin;
		ControlMap["d09f2340818511d396f6aaf844c7e325通用对话框"] = krnl_CommonDlg;
		ControlMap["d09f2340818511d396f6aaf844c7e325时钟"] = krnl_Timer;
		ControlMap["d09f2340818511d396f6aaf844c7e325打印机"] = krnl_printer;
		ControlMap["d09f2340818511d396f6aaf844c7e325数据报"] = krnl_UDP;
		ControlMap["d09f2340818511d396f6aaf844c7e325客户"] = krnl_Client;
		ControlMap["d09f2340818511d396f6aaf844c7e325服务器"] = krnl_Server;
		ControlMap["d09f2340818511d396f6aaf844c7e325端口"] = krnl_SerialPort;
		ControlMap["d09f2340818511d396f6aaf844c7e325表格"] = krnl_Grid;
		ControlMap["d09f2340818511d396f6aaf844c7e325数据源"] = krnl_DataSrc;
		ControlMap["d09f2340818511d396f6aaf844c7e325通用提供者"] = krnl_NProvider;
		ControlMap["d09f2340818511d396f6aaf844c7e325数据库提供者"] = krnl_DBProvider;
		ControlMap["d09f2340818511d396f6aaf844c7e325图形按钮"] = krnl_PicBtn;
		ControlMap["d09f2340818511d396f6aaf844c7e325外部数据库"] = krnl_ODBCDB;
		ControlMap["d09f2340818511d396f6aaf844c7e325外部数据提供者"] = krnl_ODBCProvider;

		ControlMap["{9DA96BF9CEBD45c5BFCF94CBE61671F5}拖放对象"] = krnl_DropTarget;
	}

	for (unsigned int nLibIndex = 0; nLibIndex < dwLibCount; ++nLibIndex) {
		unsigned int controlTypeId = 0;
		LIB_INFO tmpLibInfo;
		get_bytes(&tmpLibInfo, sizeof(LIB_INFO), get_dword(lpLibStartAddr));
		lpLibStartAddr = lpLibStartAddr + 4;

		//判断是否符合支持库格式
		if (tmpLibInfo.m_dwLibFormatVer != 0x1312D65) {
			continue;
		}

		mid_ELibInfo eLibInfo;
		eLibInfo.m_Name = get_shortstring(tmpLibInfo.m_lpName);
		eLibInfo.m_Guid = get_shortstring(tmpLibInfo.m_lpGuid);
		eLibInfo.m_nMajorVersion = tmpLibInfo.m_nMajorVersion;
		eLibInfo.m_nMinorVersion = tmpLibInfo.m_nMinorVersion;

		ea_t lpFirstDataType = tmpLibInfo.m_lpDataType;
		for (int nDataTypeIndex = 0; nDataTypeIndex < tmpLibInfo.m_nDataTypeCount; ++nDataTypeIndex) {
			LIB_DATA_TYPE_INFO tmpDataTypeInfo;
			get_bytes(&tmpDataTypeInfo, sizeof(LIB_DATA_TYPE_INFO), lpFirstDataType);
			lpFirstDataType += sizeof(LIB_DATA_TYPE_INFO);

			mid_EDataTypeInfo eDataType;
			if (tmpDataTypeInfo.m_lpszName) {
				controlTypeId = (nLibIndex + 1) << 0x10;
				controlTypeId = controlTypeId + (nDataTypeIndex + 1);
				eDataType.m_Name = get_shortstring(tmpDataTypeInfo.m_lpszName);

				QMap<qstring, ControlType_t>::iterator it = ControlMap.find(eLibInfo.m_Guid + eDataType.m_Name);
				if (it != ControlMap.end()) {
					GuiParser::AddControlType(controlTypeId, *it);
				}
			}
			eLibInfo.mVec_DataTypeInfo.push_back(eDataType);
		}
		m_eAppInfo.mVec_LibInfo.push_back(eLibInfo);
	}
	return true;
}

EDecompilerEngine::EDecompilerEngine()
{
	m_ProgramType = E_UNKNOWN;
}

EDecompilerEngine::~EDecompilerEngine()
{
	if (gMenu_ShowResource) {
		gMenu_ShowResource->DestroyMenu();
		gMenu_ShowResource = nullptr;
	}
	if (gMenu_ShowGUIInfo) {
		gMenu_ShowGUIInfo->DestroyMenu();
		gMenu_ShowGUIInfo = nullptr;
	}
	if (gMenu_ShowEventInfo) {
		gMenu_ShowEventInfo->DestroyMenu();
		gMenu_ShowEventInfo = nullptr;
	}
}

bool EDecompilerEngine::InitDecompilerEngine()
{
	if (!InitSectionManager()) {
		return false;
	}

	m_ProgramType = E_UNKNOWN;

	//探测易语言程序类型
	compiled_binpat_vec_t binPat;
	parse_binpat_str(&binPat, 0x0, "50 64 89 25 00 00 00 00 81 EC AC 01 00 00 53 56 57", 16);
	unsigned int segCount = get_segm_qty();
	ea_t searchAddr = BADADDR;
	for (unsigned int n = 0; n < segCount; ++n) {
		segment_t* pSegment = getnseg(n);
		searchAddr = bin_search2(pSegment->start_ea, pSegment->end_ea, binPat, 0x0);
		if (searchAddr != BADADDR) {
			break;
		}
	}

	if (searchAddr != BADADDR) {
		m_ProgramType = E_STATIC;
		m_EHeadAddr = get_dword(searchAddr + 0x26);
	}
	
	//To do...即便是静态编译特征被VM了,应该还有一些解决的思路,暂时先不管。

	return true;
}

bool EDecompilerEngine::DoDecompile()
{
	//静态编译程序
	if (m_ProgramType == E_STATIC) {
		return DoDecompiler_EStatic();
	}

	return true;
}

bool EDecompilerEngine::ParseKrnlInterface(ea_t lpKrnlEntry)
{
	lpKrnlEntry -= sizeof(mid_KrnlApp);
	get_bytes(&m_eAppInfo.m_KrnlApp, sizeof(mid_KrnlApp), lpKrnlEntry);
	return true;
}

bool EDecompilerEngine::DoDecompiler_EStatic()
{
	EHead eHead;
	get_bytes(&eHead, sizeof(EHead), m_EHeadAddr);
	if (eHead.dwMagic != 0x3) {
		return false;
	}

	m_eAppInfo.m_UserCodeStartAddr = eHead.lpStartCode;
	m_eAppInfo.m_UserCodeEndAddr = m_EHeadAddr;
	GuiParser::InitUserCodeStartAddr(m_eAppInfo.m_UserCodeStartAddr);
	UserResourceParser::InitUserCodeAddr(m_eAppInfo.m_UserCodeStartAddr, m_eAppInfo.m_UserCodeEndAddr);

	if (!eHead.dwLibNum || !eHead.lpLibEntry) {
		return false;
	}

	ParseLibInfomation(eHead.lpLibEntry, eHead.dwLibNum);


	ea_t dwKrnlEntry = eHead.lpEString;
	if (dwKrnlEntry == 0) {
		dwKrnlEntry = eHead.lpEWindow;
	}
	ParseKrnlInterface(dwKrnlEntry);

	if (eHead.lpEString != 0 && eHead.dwEStringSize != 0) {
		UserResourceParser::ParseUserResource(eHead.lpEString, eHead.dwEStringSize);
		gMenu_ShowResource = IDAMenu::CreateMenu(getUTF8String("易语言/用户常量资源").c_str(), UserResourceParser::MenuHandle_ShowUserResource);
	}

	if (eHead.lpEWindow != 0 && eHead.dwEWindowSize > 4) {
		GuiParser::ParseGUIResource(eHead.lpEWindow, eHead.dwEWindowSize);
		gMenu_ShowGUIInfo = IDAMenu::CreateMenu(getUTF8String("易语言/窗口控件信息").c_str(), GuiParser::MenuHandle_ShowGuiInfo);
		if (GuiParser::GetEventCount()) {
			gMenu_ShowEventInfo = IDAMenu::CreateMenu(getUTF8String("易语言/控件事件信息").c_str(), GuiParser::MenuHandle_ShowEventInfo);
		}
	}

	
	msg("%s\n", getUTF8String("检测到是易语言静态编译程序").c_str());
	return true;
}
