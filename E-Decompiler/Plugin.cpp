#include "Plugin.h"
#include <hexrays.hpp>
#include <auto.hpp>
#include "EDecompiler.h"
#include "./common/public.h"

hexdsp_t* hexdsp = NULL;


Plugin::Plugin()
{
	msg("[E-Decompiler] plugin 0.1 loaded,Author: fjqisba\n");
}

Plugin::~Plugin()
{
	term_hexrays_plugin();
}

//--------------------------------------------------------------------------
bool idaapi Plugin::run(size_t args)
{
	show_wait_box(getUTF8String("等待IDA初始化分析完毕").c_str());
	auto_wait();
	hide_wait_box();
	if (!EDecompiler::Instance().InitDecompilerEngine()) {
		msg("Error,InitDecompilerEngine Failed\n");
		return false;
	}
	return true;
}


//--------------------------------------------------------------------------
static plugmod_t* idaapi init()
{
	// no decompiler
	if (!init_hexrays_plugin()) {
		return nullptr;
	}
	return new Plugin();
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


