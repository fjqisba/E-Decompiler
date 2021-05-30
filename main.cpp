#include <hexrays.hpp>
#include <auto.hpp>
#include "SectionManager.h"
#include "EDecompiler.h"
#include <kernwin.hpp>
#include "common/public.h"
#include <QTextCodec>

// Hex-Rays API pointer
hexdsp_t* hexdsp = nullptr;

//--------------------------------------------------------------------------
struct plugin_ctx_t : public plugmod_t
{
	bool inited = true;

	~plugin_ctx_t()
	{
		if (inited) {
			term_hexrays_plugin();
		}
		unhook_from_notification_point(HT_UI, g_MyDecompiler.ui_callback);
	}
	virtual bool idaapi run(size_t) override;
};


//--------------------------------------------------------------------------
static plugmod_t* idaapi init()
{
	if (!init_hexrays_plugin())
		return nullptr; // no decompiler
	const char* hxver = get_hexrays_version();
	hook_to_notification_point(HT_UI, g_MyDecompiler.ui_callback);
	return new plugin_ctx_t;
}

//--------------------------------------------------------------------------
bool idaapi plugin_ctx_t::run(size_t)
{
	show_wait_box(getUTF8String("等待IDA初始化分析完毕").c_str());
	auto_wait();
	hide_wait_box();

	if (!g_MyDecompiler.InitDecompilerEngine()) {
		msg("Error,InitDecompilerEngine Failed\n");
		return false;
	}

	return true;
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
