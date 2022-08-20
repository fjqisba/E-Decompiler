#include "EDecompiler.h"
#include <pro.h>
#include <bytes.hpp>
#include <segment.hpp>
#include <loader.hpp>
#include <funcs.hpp>
#include <typeinf.hpp>
#include <ua.hpp>
#include <auto.hpp>
#include <allins.hpp>
#include <diskio.hpp>
#include <struct.hpp>
#include "common/IDAMenu.h"
#include "common/public.h"
#include "ControlInfoWidget.h"

#include "GuiParser.h"
#include "EsigScanner.h"
#include "ECSigParser.h"
#include "ImportsParser.h"

#define ACTION_GenECSig "eDecompiler::GenerateECSig"


struct GenECSigHandler :public action_handler_t
{
	int idaapi activate(action_activation_ctx_t* ctx)
	{
		return ECSigParser::GenerateECSig(get_screen_ea());
	}
	action_state_t idaapi update(action_update_ctx_t* ctx)
	{
		return AST_ENABLE_ALWAYS;
	}
};

struct DecompileHandler :public action_handler_t
{
	int idaapi activate(action_activation_ctx_t* ctx)
	{
		return EDecompiler::Instance().DoDecompile();
	}
	action_state_t idaapi update(action_update_ctx_t* ctx)
	{
		return AST_ENABLE_ALWAYS;
	}
};

DecompileHandler gHandler_Decompile;
GenECSigHandler gHandler_GenEcSig;


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

EDecompiler::EDecompiler() :cTreeFixer(eSymbol)
{
	arch = E_STATIC;
	hook_to_notification_point(HT_UI, ui_callback);
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
	unhook_from_notification_point(HT_UI, ui_callback);
}


EDecompiler& EDecompiler::Instance()
{
	static EDecompiler g_Decompiler;
	return g_Decompiler;
}

ssize_t EDecompiler::ui_callback(void* ud, int notification_code, va_list va)
{
	if (notification_code == ui_populating_widget_popup) {
		TWidget* view = va_arg(va, TWidget*);
		if (get_widget_type(view) == BWN_DISASM) {
			TPopupMenu* p = va_arg(va, TPopupMenu*);
			attach_action_to_popup(view, p, "eDecompiler::GenerateECSig", nullptr, SETMENU_FIRST);
		}
	}
	return 0;
}

bool EDecompiler::InitDecompilerEngine()
{
	FixEStructure();
	if (!SectionManager::InitSectionManager()) {
		return false;
	}
	if (!InitEArchitectureType()) {
		return false;
	}

	//注册窗口菜单
	qstring menuName = getUTF8String("生成易语言函数特征");
	const action_desc_t GenEsigDesc = {
	sizeof(action_desc_t),
	ACTION_GenECSig,
	menuName.c_str(),
	&gHandler_GenEcSig,
	&PLUGIN,
	nullptr,
	nullptr,
	0,
	ADF_OT_PLUGIN
	};
	register_action(GenEsigDesc);

	//注册快捷键
	const action_desc_t desc = {
	sizeof(action_desc_t),
	"eDecompile",
	"e Decompile",
	&gHandler_Decompile,
	&PLUGIN,
	"F6",
	nullptr,
	0,
	ADF_OT_PLUGIN
	};
	register_action(desc);
	return true;
}

void EDecompiler::FixEStructure()
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

bool EDecompiler::DoDecompile()
{
	ea_t funcAddr = get_screen_ea();

	func_t* pFunc = get_func(funcAddr);
	if (!pFunc) {
		return false;
	}


	return true;
}


bool EDecompiler::InitEArchitectureType()
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






#ifdef _DEBUG
	ECSigParser::Debug_outputECSig();
#endif

	//识别易语言模块函数
	//show_wait_box(getUTF8String("识别模块函数").c_str());
	//qstring mainECpath;
	//mainECpath.sprnt("%s\\esig\\精易模块.msig", idadir(PLG_SUBDIR));
	//ECSigParser::ScanMSig(mainECpath.c_str(), m_eAppInfo.m_UserCodeStartAddr, m_eAppInfo.m_UserCodeEndAddr);
	//hide_wait_box();
	//msg("%s\n", getUTF8String("检测到是易语言静态编译程序").c_str());
	return true;
}
