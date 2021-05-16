#include "DecompilerCore.h"
#include "ControlFlowGraph.h"
#include <funcs.hpp>
#include <ua.hpp>
#include <allins.hpp>


bool DecompilerCore::Decompile(func_t* pFunc)
{
	FastDeEngine engine;
	engine.DoFastDecompile(pFunc);

	return true;
}