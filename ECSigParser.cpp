#include "ECSigParser.h"
#include <funcs.hpp>
#include <bytes.hpp>
#include <segment.hpp>
#include <name.hpp>
#include <ua.hpp>
#include <allins.hpp>
#include <auto.hpp>
#include "SectionManager.h"
#include "EDecompiler.h"
#include "common/public.h"

mid_KrnlJmp ECSigParser::m_KrnlJmp;

const char* GetDataType(uint8 n)
{
	switch (n)
	{
	case 0:
		return "BYTE";
	case 1:
		return "WORD";
	case 2:
		return "DWORD";
	default:
		break;
	}
	return NULL;
}

const char* GetRegName(uint16 reg)
{
	switch (reg)
	{
	case 0:
		return "EAX";
	case 1:
		return "ECX";
	case 2:
		return "EDX";
	case 3:
		return "EBX";
	case 4:
		return "ESP";
	case 5:
		return "EBP";
	case 6:
		return "ESI";
	case 7:
		return "EDI";
	}

	msg("[GetRegName]To do...\n");
	return NULL;
}

qstring GetInsPatternHex_Bak(insn_t& ins, char offset)
{
	qstring ret;
	unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
	for (char n = 0; n < ins.size; ++n) {
		if (n < offset) {
			ret.append(UCharToStr(pData[n]));
		}
		else {
			ret.append("??");
		}
	}
	return ret;
}

qstring GetInstructionHexStr(insn_t& ins)
{
	qstring ret;
	unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
	for (unsigned int n = 0; n < ins.size; ++n) {
		ret.append(UCharToStr(pData[n]));
	}
	return ret;
}


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

qstring ECSigParser::GetSig_Call(insn_t& ins)
{
	qstring ret;
	
	if (ins.ops[0].type == o_near) {
		if (ins.ops[0].addr == m_KrnlJmp.Jmp_MCallKrnlLibCmd) {
			insn_t LastIns;
			decode_prev_insn(&LastIns, ins.ip);
			if (LastIns.itype == NN_mov && LastIns.ops[0].reg == 3 && LastIns.ops[1].type == o_imm) {
				qstring KrnlLibName = get_name(LastIns.ops[1].value);
				if (KrnlLibName.substr(0, 4) == "sub_") {
					msg("[GetSig_Call]Function not Scanned,%a", LastIns.ops[1].value);
					ret = "<未知命令>";
					return ret;
				}
				ret.sprnt("<%s>", KrnlLibName.c_str());
				return ret;
			}
		}
	}
	int a = 0;

	msg("[GetSig_Call]To do...\n");
	return ret;
}

qstring ECSigParser::GetSig_Sub(insn_t& ins)
{
	qstring ret;

	if (ins.ops[0].type == o_reg) {
		if (ins.ops[1].type == o_imm) {
			ret = GetInsPatternHex_Bak(ins, ins.ops[1].offb);
			return ret;
		}
	}

	msg("[GetSig_Sub]To do...\n");
	return ret;
}

qstring ECSigParser::GetSig_Mov(insn_t& ins)
{
	qstring ret;
	if (ins.ops[0].type == o_reg) {
		if (ins.ops[1].type == o_reg) {
			ret = GetInstructionHexStr(ins);
			return ret;
		}
		if (ins.ops[1].type == o_phrase) {
			ret = GetInstructionHexStr(ins);
			return ret;
		}
		if (ins.ops[1].type == o_displ) {
			//To do...
			ret = GetInstructionHexStr(ins);
			return ret;
		}

		//mov eax,0x401000
		if (ins.ops[1].type == o_imm) {
			ret = GetInsPatternHex_Bak(ins, ins.ops[1].offb);
			return ret;
		}
	}

	if (ins.ops[0].type == o_phrase)
	{
		if (ins.ops[1].type == o_reg) {
			ret = GetInstructionHexStr(ins);
			return ret;
		}
	}

	if (ins.ops[0].type == o_displ) {
		if (ins.ops[1].type == o_imm) {
			ret = GetInsPatternHex_Bak(ins, ins.ops[1].offb);
			return ret;
		}
	}
	
	msg("[GetSig_Mov]To do...\n");
	return ret;
}

qstring ECSigParser::GetSig_Push(insn_t& ins)
{
	qstring ret;

	//push reg
	if (ins.ops[0].type == o_reg) {
		ret = GetInstructionHexStr(ins);
		return ret;
	}
	
	if (ins.ops[0].type == o_imm) {
		ret = GetInsPatternHex_Bak(ins, ins.ops[0].offb);
		return ret;
	}
	msg("[GetSig_Push]To do...\n");
	return ret;
}

void ECSigParser::InitECSigParser(mid_KrnlJmp& inFunc)
{
	m_KrnlJmp = inFunc;
}

int ECSigParser::GenerateECSig(ea_t addr)
{
	if (!auto_is_ok()) {
		info(getUTF8String("请等待IDA分析完毕...").c_str());
		return false;
	}

	func_t* pFunc = get_func(addr);
	if (!pFunc) {
		return false;
	}

	if (!IsEStandardFunction(pFunc->start_ea)) {
		msg("%s\n", getUTF8String("暂不支持此类易语言函数").c_str());
		return false;
	}

	ea_t startAddr = pFunc->start_ea;
	ea_t endAddr = SeachEFuncEnd(startAddr);
	if (endAddr == BADADDR) {
		msg("%s\n", getUTF8String("寻找函数尾部失败").c_str());
		return false;
	}

	qstring STRING_SIG;
	do
	{
		qstring SingleSig;
		insn_t CurrentIns;
		int insLen = decode_insn(&CurrentIns, startAddr);
		if (!insLen) {
			msg("%s\n", getUTF8String("反汇编指令失败").c_str());
			return false;
		}

		switch (CurrentIns.itype)
		{
		case NN_sub:
			SingleSig = GetSig_Sub(CurrentIns);
			break;
		case NN_push:
			SingleSig = GetSig_Push(CurrentIns);
			break;
		case NN_mov:
			SingleSig = GetSig_Mov(CurrentIns);
			break;
		case NN_call:
			SingleSig = GetSig_Call(CurrentIns);
			break;
		default:
			SingleSig = GetInstructionHexStr(CurrentIns);
			break;
		}

		if (SingleSig.empty()) {
			msg("%s--%a\n", getUTF8String("获取特征失败").c_str(), startAddr);
		}

		startAddr = startAddr + CurrentIns.size;
		STRING_SIG.append(SingleSig);
	} while (startAddr <= endAddr);

	qstring finalMD5 = CalculateMD5(STRING_SIG);
	msg("[SIGSTRING]:%s\n", STRING_SIG.c_str());
	msg("[SIGMD5]:%s\n", finalMD5.c_str());
	int a = 0;
	return true;
}