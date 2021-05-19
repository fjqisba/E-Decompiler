#include "ECSigParser.h"
#include <funcs.hpp>
#include <bytes.hpp>
#include <segment.hpp>
#include <ua.hpp>
#include <allins.hpp>
#include "public.h"

ea_t ECSigParser::SeachEFuncEnd(ea_t startAddr)
{
	ea_t ret = BADADDR;

	compiled_binpat_vec_t binPat;
	parse_binpat_str(&binPat, 0x0, "8B E5 5D", 16);
	segment_t* pSegment = getseg(startAddr);
	if (!pSegment) {
		return BADADDR;
	}

	while (true) {
		ret = bin_search2(startAddr, pSegment->end_ea, binPat, 0x0);
		if (ret == BADADDR) {
			return BADADDR;
		}
		ret = ret + 3;
		insn_t tmpIns;
		if (!decode_insn(&tmpIns, ret)) {
			return BADADDR;
		}
		if (tmpIns.itype == NN_retn) {
			break;
		}
		startAddr = ret;
	}
	return ret;
}

bool ECSigParser::IsEStandardFunction(ea_t startAddr)
{
	unsigned int head = get_dword(startAddr);
	if ((head & 0xFFFFFF) == 0xEC8B55) {
		return true;
	}
	return false;
}

int ECSigParser::GenerateECSig(ea_t addr)
{
	func_t* pFunc = get_func(addr);
	if (!pFunc) {
		return false;
	}

	if (!IsEStandardFunction(pFunc->start_ea)) {
		msg("%s\n", getUTF8String("暂不支持此类易语言函数").c_str());
		return false;
	}
	
	ea_t endAddr = SeachEFuncEnd(pFunc->start_ea);
	int a = 0;
	return true;
}