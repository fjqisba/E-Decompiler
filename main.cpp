#include <hexrays.hpp>
#include <auto.hpp>
#include "SectionManager.h"
#include "EDecompiler.h"
#include <kernwin.hpp>
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
	}
	virtual bool idaapi run(size_t) override;
};

//--------------------------------------------------------------------------
static plugmod_t* idaapi init()
{
	if (!init_hexrays_plugin())
		return nullptr; // no decompiler
	const char* hxver = get_hexrays_version();
	return new plugin_ctx_t;
}

//--------------------------------------------------------------------------
bool idaapi plugin_ctx_t::run(size_t)
{
	if (!auto_is_ok() && ask_yn(0, "The autoanalysis has not finished yet.\nDo you want to continue?") < 1)
	{
		return true;
	}

	if (!g_MyDecompiler.InitDecompilerEngine()) {
		msg("Error,InitDecompilerEngine Failed\n");
		return false;
	}

	if (!g_MyDecompiler.DoDecompile()) {
		msg("Error,DoDecompile\n");
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
