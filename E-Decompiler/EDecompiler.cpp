#include "EDecompiler.h"

#include <auto.hpp>
#include "Utils/IDAMenu.h"
#include "common/public.h"
#include "EsigScanner.h"
#include "ECSigParser.h"
#include "ImportsParser.h"
#include <hexrays.hpp>
#include "./Module/ShowEventList.h"

hexdsp_t* hexdsp = NULL;

ssize_t PluginUI_Callback(void* ud, int notification_code, va_list va)
{
	EDecompiler* decompiler = (EDecompiler*)(ud);
	if (notification_code == ui_populating_widget_popup) {
		TWidget* view = va_arg(va, TWidget*);
		TPopupMenu* p = va_arg(va, TPopupMenu*);
		int widgetType = get_widget_type(view);
		if (BWN_DISASM == widgetType) {
			decompiler->ecSigMaker.AttachToPopupMenu(view, p);
		}
		else if (BWN_PSEUDOCODE == widgetType) {
			decompiler->eControlXref.AttachToPopupMenu(view,p);
		}
	}
	return 0;
}

void EDecompiler::makeFunction(ea_t startAddr, ea_t endAddr)
{
	compiled_binpat_vec_t FuncHeadBin;
	parse_binpat_str(&FuncHeadBin, 0, "55 8B EC", 16);
	while (true)
	{
		startAddr = bin_search2(startAddr, endAddr, FuncHeadBin, 0x0);
		if (startAddr == BADADDR)
		{
			break;
		}
		add_func(startAddr);
		startAddr = startAddr + 3;
	}
}

EDecompiler::EDecompiler() :cTreeFixer(eSymbol),eControlXref(eSymbol)
{
	arch = E_STATIC;
	hook_to_notification_point(HT_UI, PluginUI_Callback, this);

	msg("[E-Decompiler] plugin 0.1 loaded,Author: fjqisba\n");
}

EDecompiler::~EDecompiler()
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
	if (gMenu_ShowImportsInfo) {
		gMenu_ShowImportsInfo->DestroyMenu();
		gMenu_ShowImportsInfo = nullptr;
	}
	term_hexrays_plugin();
	unhook_from_notification_point(HT_UI, PluginUI_Callback,this);
}


bool idaapi EDecompiler::run(size_t)
{
	show_wait_box(getUTF8String("等待IDA初始化分析完毕").c_str());
	auto_wait();
	hide_wait_box();
	if (!this->InitDecompilerEngine()) {
		msg("Error,InitDecompilerEngine Failed\n");
		return false;
	}
	return true;
}

bool EDecompiler::InitDecompilerEngine()
{
	ImportsEStructure();
	if (!SectionManager::InitSectionManager()) {
		return false;
	}
	if (!initEArchitectureType()) {
		return false;
	}

	ecSigMaker.RegisterAction(this);
	eControlXref.RegisterAction(this);

	gMenu_ShowEventInfo = IDAMenu::CreateMenu(getUTF8String("易语言/控件事件信息").c_str(), ShowEventList, &eSymbol);
	return true;
}

void EDecompiler::ImportsEStructure()
{
	auto idati = (til_t*)get_idati();
	
	const char *krnlRet = "\
	union krnlRet\
	{\
		__int64 i;\
		char *c;\
		double d;\
	};";
	parse_decls(idati, krnlRet, NULL, 0);

	const char* UNIT_PROPERTY_VALUE = "\
	union UNIT_PROPERTY_VALUE\
	{\
		unsigned int i;\
		char *c;\
	};";
	parse_decls(idati, UNIT_PROPERTY_VALUE, NULL, 0);
}

bool EDecompiler::initEArchitectureType()
{
	this->arch = E_UNKNOWN;

	//探测易语言程序类型
	ea_t staticMagicHead = SectionManager::SeachBin("50 64 89 25 00 00 00 00 81 EC AC 01 00 00 53 56 57");
	if (staticMagicHead != BADADDR) {
		unsigned int eHeadAddr = get_dword(staticMagicHead + 0x26);
		if (Parse_EStatic(eHeadAddr)) {
			this->arch = E_STATIC;
			return true;
		}
	}

	//To do...即便是静态编译特征被VM了,应该还有一些解决的思路,暂时先不管。
	return false;
}

bool EDecompiler::Parse_EStatic(unsigned int eHeadAddr)
{
	EComHead eHead;
	get_bytes(&eHead, sizeof(EComHead), eHeadAddr);
	if (eHead.dwMagic != 0x3) {
		return false;
	}

	eSymbol.userCodeStartAddr = eHead.lpStartCode;
	eSymbol.userCodeEndAddr = eHeadAddr;

	show_wait_box(getUTF8String("扫描易语言函数").c_str());
	makeFunction(eSymbol.userCodeStartAddr, eSymbol.userCodeEndAddr);
	auto_wait();
	hide_wait_box();

	return eSymbol.LoadEStaticSymbol(eHeadAddr,&eHead);

	//识别易语言模块函数
	//show_wait_box(getUTF8String("识别模块函数").c_str());
	//qstring mainECpath;
	//mainECpath.sprnt("%s\\esig\\精易模块.msig", idadir(PLG_SUBDIR));
	//ECSigParser::ScanMSig(mainECpath.c_str(), m_eAppInfo.m_UserCodeStartAddr, m_eAppInfo.m_UserCodeEndAddr);
	//hide_wait_box();
	//msg("%s\n", getUTF8String("检测到是易语言静态编译程序").c_str());
	return true;
}

static plugmod_t* idaapi init()
{
	// no decompiler
	if (!init_hexrays_plugin()) {
		return nullptr;
	}
	return new EDecompiler();
}

//--------------------------------------------------------------------------
static char comment[] = "E-Language Programs Decompiler By fjqisba";

#define PLUGINNAME "\xE6\x98\x93\xE8\xAF\xAD\xE8\xA8\x80\xE5\x8F\x8D\xE7\xBC\x96\xE8\xAF\x91\xE5\x99\xA8"

plugin_t PLUGIN =
{
  IDP_INTERFACE_VERSION,
  PLUGIN_MULTI ,                // The plugin can work with multiple idbs in parallel
  init,                         // initialize
  nullptr,
  nullptr,
  comment,              // long comment about the plugin
  "fjqisba@sohu.com",   // multiline help about the plugin
  PLUGINNAME,           // the preferred short name of the plugin
  nullptr,              // the preferred hotkey to run the plugin
};

