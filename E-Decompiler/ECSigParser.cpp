#include "ECSigParser.h"
#include <funcs.hpp>
#include <bytes.hpp>
#include <segment.hpp>
#include <name.hpp>
#include <ua.hpp>
#include <allins.hpp>
#include <auto.hpp>
#include <fpro.h>
#include <diskio.hpp>
#include <search.hpp>
#include "SectionManager.h"
#include "ImportsParser.h"
#include "EDecompiler.h"
#include "common/public.h"

eSymbol_KrnlJmp ECSigParser::m_KrnlJmp;
std::map<ea_t, qstring> ECSigParser::mMap_BasicFunc;
uint32 ECSigParser::m_UserResourceStartAddr = 0x00000000;
uint32 ECSigParser::m_UserResourceEndAddr = 0x0;
std::map<ea_t, qstring> ECSigParser::mSave_SubFunc;
bool ECSigParser::bFuzzySig = false;

bool isJumpInst(uint16 itype)
{
	if (itype >= NN_ja && itype <= NN_jmpshort) {
		return true;
	}
	return false;
}

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


qstring ECSigParser::GetInsPattern_Three(insn_t& ins)
{
	qstring ret;
	unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
	for (char n = 0; n < ins.ops[0].offb; ++n) {
		ret.append(UCharToStr(pData[n]));
	}
	if (IsUserResourceOffset(ins.ops[0].addr)) {
		for (char n = ins.ops[0].offb; n < ins.ops[1].offb; ++n) {
			ret.append("??");
		}
	}
	else {
		for (char n = ins.ops[0].offb; n < ins.ops[1].offb; ++n) {
			ret.append(UCharToStr(pData[n]));
		}
	}

	if (IsUserResourceImm(ins.ops[1].value)) {
		for (char n = ins.ops[1].offb; n < ins.size; ++n) {
			ret.append("??");
		}
	}
	else {
		for (char n = ins.ops[1].offb; n < ins.size; ++n) {
			ret.append(UCharToStr(pData[n]));
		}
	}

	return ret;
}

qstring ECSigParser::GetInsPattern_Two(insn_t& ins, char offset)
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

qstring GetInsHex(insn_t& ins)
{
	qstring ret;
	unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
	for (unsigned int n = 0; n < ins.size; ++n) {
		ret.append(UCharToStr(pData[n]));
	}
	return ret;
}

qstring ECSigParser::GetSig_LongJmp(insn_t& ins)
{
	qstring ret;
	unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);

	//长跳转统一模糊处理
	if (ins.size >= 5) {
		ret.append(UCharToStr(pData[0]));
		return ret;
	}

	ret = GetInsHex(ins);
	return ret;
}

bool ECSigParser::IsUserResourceOffset(uint32 offset)
{
	int32 distance = (uint32)offset;
	if (distance < 0) {
		offset = -offset;
	}
	return IsUserResourceImm(offset);
}

bool ECSigParser::IsUserResourceImm(uint32 imm)
{
	if (imm >= m_UserResourceStartAddr && imm < m_UserResourceEndAddr) {
		return true;
	}
	return false;
}

qstring ECSigParser::GetSig_Nop(insn_t& ins)
{
	qstring ret;

	if (ins.ops[0].type == o_void) {
		ret = GetInsHex(ins);
		return ret;
	}
	

	msg("[GetSig_Nop]To do...\n");
	return ret;
}

qstring ECSigParser::GetSig_Call(insn_t& ins, qvector<qstring>& vec_saveSig,bool& out_bSkipState)
{
	qstring ret;
	if (ins.ops[0].type == o_near) {
		//错误回调函数
		if (ins.ops[0].addr == m_KrnlJmp.Jmp_MReportError) {
			ea_t lastInsAddr = ins.ip;
			out_bSkipState = true;
			insn_t LastIns;
			lastInsAddr = decode_prev_insn(&LastIns, lastInsAddr);
			if (LastIns.itype == NN_push && LastIns.ops[0].type == o_imm) {
				vec_saveSig[vec_saveSig.size() - 1] = "";
			}
			lastInsAddr = decode_prev_insn(&LastIns, lastInsAddr);
			if (LastIns.itype == NN_push && LastIns.ops[0].type == o_imm) {
				vec_saveSig[vec_saveSig.size() - 2] = "";
			}
			else if (isJumpInst(LastIns.itype)) {
				vec_saveSig[vec_saveSig.size() - 2] = "";
				return getUTF8String("<错误回调>");
			}
			lastInsAddr = decode_prev_insn(&LastIns, lastInsAddr);
			if (LastIns.itype == NN_push && LastIns.ops[0].type == o_imm) {
				vec_saveSig[vec_saveSig.size() - 3] = "";
				vec_saveSig[vec_saveSig.size() - 4] = "";
			}
			return getUTF8String("<错误回调>");
		}
		//调用DLL命令
		if (ins.ops[0].addr == m_KrnlJmp.Jmp_MCallDllCmd) {
			insn_t LastIns;
			decode_prev_insn(&LastIns, ins.ip);
			//判断上一条指令是不是mov eax,DLL序号
			if (LastIns.itype == NN_mov && LastIns.ops[0].reg == 0 && LastIns.ops[1].type == o_imm) {
				vec_saveSig[vec_saveSig.size() - 1] = "B8????????";
				qstring DLLFuncName = ImportsParser::mVec_ImportsApi[LastIns.ops[1].value].ApiName;
				ret.sprnt("[%s]", DLLFuncName.c_str());
				return ret;
			}
		}
		//系统核心支持库或者三方支持库
		if (ins.ops[0].addr == m_KrnlJmp.Jmp_MCallKrnlLibCmd || ins.ops[0].addr==m_KrnlJmp.Jmp_MCallLibCmd) {
			insn_t LastIns;
			decode_prev_insn(&LastIns, ins.ip);
			//判断上一条指令是不是mov ebx,命令地址
			if (LastIns.itype == NN_mov && LastIns.ops[0].reg == 3 && LastIns.ops[1].type == o_imm) {
				vec_saveSig[vec_saveSig.size() - 1] = "BB????????";
				if (bFuzzySig) {
					return getUTF8String("<未知命令>");
				}
				qstring KrnlLibName = get_name(LastIns.ops[1].value);
				if (KrnlLibName.substr(0, 4) == "sub_") {
					msg("[GetSig_Call]Function not Scanned,%a", LastIns.ops[1].value);
					ret = getUTF8String("<未知命令>");
					return ret;
				}
				ret.sprnt("<%s>", KrnlLibName.c_str());
				return ret;
			}
		}
		if (ins.ops[0].addr == m_KrnlJmp.Jmp_MReadProperty) {
			ret = getUTF8String("<读取组件属性>");
			return ret;
		}
		if (ins.ops[0].addr == m_KrnlJmp.Jmp_MWriteProperty) {
			ret = getUTF8String("<设置组件属性>");
			return ret;
		}
		if (ins.ops[0].addr == m_KrnlJmp.Jmp_MMalloc) {
			ret = getUTF8String("<分配内存>");
			return ret;
		}
		if (ins.ops[0].addr == m_KrnlJmp.Jmp_MRealloc) {
			ret = getUTF8String("<重新分配内存>");
			return ret;
		}
		if (ins.ops[0].addr == m_KrnlJmp.Jmp_MFree) {
			ret = getUTF8String("<释放内存>");
			return ret;
		}
		if (ins.ops[0].addr == m_KrnlJmp.Jmp_MExitProcess) {
			ret = getUTF8String("<结束>");
			return ret;
		}
		if (ins.ops[0].addr == m_KrnlJmp.Jmp_MOtherHelp) {
			ret = getUTF8String("<辅助函数>");
			return ret;
		}
		//call $addr+0x5
		if (ins.ip + 5 == ins.ops[0].addr) {
			return GetInsHex(ins);
		}
		auto it = mMap_BasicFunc.find(ins.ops[0].addr);
		if (it != mMap_BasicFunc.end()) {
			ret.sprnt("<%s>", getUTF8String(it->second.c_str()).c_str());
			return ret;
		}
		//用户自定义函数
		qstring subFuncName = mSave_SubFunc[ins.ops[0].addr];
		if (subFuncName.empty()) {
			//防止递归循环
			mSave_SubFunc[ins.ops[0].addr] = "<RecurFunc>";
			subFuncName = GetFunctionMD5(ins.ops[0].addr);
			if (subFuncName.empty()) {
				//表示该函数其实是置入代码
				ret = GetInsHex(ins);
				mSave_SubFunc[ins.ops[0].addr] = ret;
				return ret;
			}
			ret.sprnt("<%s>", subFuncName.c_str());
			mSave_SubFunc[ins.ops[0].addr] = ret;
			return ret;
		}
		ret.sprnt("%s", subFuncName.c_str());
		return ret;
	}
	
	msg("[GetSig_Call]To do...\n");
	return ret;
}

qstring ECSigParser::GetSig_FloatInstA(insn_t& ins)
{
	qstring ret;
	if (ins.ops[0].type == o_reg) {
		return GetInsHex(ins);
	}

	if (ins.ops[0].type == o_mem) {
		unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
		for (char n = 0; n < ins.size; ++n) {
			if (n >= ins.ops[0].offb) {
				ret.append("??");
			}
			else {
				ret.append(UCharToStr(pData[n]));
			}
		}
		return ret;
	}

	if (ins.ops[0].type == o_phrase) {
		return GetInsHex(ins);
	}

	//fadd [ebp-0xC]
	if (ins.ops[0].type == o_displ) {
		if (IsUserResourceOffset(ins.ops[0].addr)) {
			unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
			for (char n = 0; n < ins.size; ++n) {
				if (n >= ins.ops[0].offb) {
					ret.append("??");
				}
				else {
					ret.append(UCharToStr(pData[n]));
				}
			}
		}
		else {
			ret = GetInsHex(ins);
		}
		return ret;
	}
	msg("[GetSig_FloatInstA]To do...\n");
	return ret;
}

qstring ECSigParser::GetSig_FloatInstB(insn_t& ins)
{
	qstring ret;
	if (ins.ops[1].type == o_mem) {
		unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
		for (char n = 0; n < ins.size; ++n) {
			if (n >= ins.ops[1].offb) {
				ret.append("??");
			}
			else {
				ret.append(UCharToStr(pData[n]));
			}
		}
		return ret;
	}

	//fadd [ebp-0xC]
	if (ins.ops[1].type == o_displ) {
		if (IsUserResourceOffset(ins.ops[1].addr)) {
			unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
			for (char n = 0; n < ins.size; ++n) {
				if (n >= ins.ops[1].offb) {
					ret.append("??");
				}
				else {
					ret.append(UCharToStr(pData[n]));
				}
			}
		}
		else {
			ret = GetInsHex(ins);
		}
		return ret;
	}
	return GetInsHex(ins);
}


qstring ECSigParser::GetSig_FlexDoubleInst(insn_t& ins)
{
	qstring ret;
	ret.reserve(ins.size * 2 + 1);

	if (ins.ops[0].type == o_reg) {
		//mov eax,ebx
		//mov eax,[eax+ebx]
		if (ins.ops[1].type == o_reg || ins.ops[1].type == o_phrase) {
			ret = GetInsHex(ins);
			return ret;
		}

		// mov eax,[0x401000]
		if (ins.ops[1].type == o_mem) {
			unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
			for (char n = 0; n < ins.size; ++n) {
				if (n >= ins.ops[1].offb) {
					ret.append("??");
				}
				else {
					ret.append(UCharToStr(pData[n]));
				}
			}
			return ret;
		}

		//mov eax,[eax+ebx-0x401000]
		if (ins.ops[1].type == o_displ) {
			if (IsUserResourceOffset(ins.ops[1].addr)) {
				unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
				for (char n = 0; n < ins.size; ++n) {
					if (n >= ins.ops[1].offb) {
						ret.append("??");
					}
					else {
						ret.append(UCharToStr(pData[n]));
					}
				}
			}
			else {
				ret = GetInsHex(ins);
			}
			return ret;
		}

		//mov eax,0x401000
		if (ins.ops[1].type == o_imm) {
			goto label_HEX_PAT;
		}
	}

	if (ins.ops[0].type == o_mem) {
		//mov [0x401000],eax
		if (ins.ops[1].type == o_reg) {
			unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
			for (char n = 0; n < ins.size; ++n) {
				if (n >= ins.ops[0].offb) {
					ret.append("??");
				}
				else {
					ret.append(UCharToStr(pData[n]));
				}
			}
			return ret;
		}
		//mov [0x401000],0x501000
		if (ins.ops[1].type == o_imm) {
			unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
			if (IsUserResourceImm(ins.ops[1].value)) {
				for (char n = 0; n < ins.size; ++n) {
					if (n >= ins.ops[0].offb) {
						ret.append("??");
					}
					else {
						ret.append(UCharToStr(pData[n]));
					}
				}
			}
			else {
				for (char n = 0; n < ins.size; ++n) {
					if (n >= ins.ops[0].offb && n < ins.ops[1].offb) {
						ret.append("??");
					}
					else {
						ret.append(UCharToStr(pData[n]));
					}
				}
			}
			return ret;
		}
	}

	if (ins.ops[0].type == o_phrase) {
		//mov [eax+ebx],eax
		if (ins.ops[1].type == o_reg) {
			return GetInsHex(ins);
		}
		//mov [eax+ebx],0x401000
		if (ins.ops[1].type == o_imm) {
			goto label_HEX_PAT;
		}
	}
	
	if (ins.ops[0].type == o_displ) {

		//mov [ebp+ebx+0x401000],eax 
		if (ins.ops[1].type == o_reg) {
			if(IsUserResourceOffset(ins.ops[0].addr)) {
				unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
				for (char n = 0; n < ins.size; ++n) {
					if (n >= ins.ops[0].offb && n < ins.ops[1].offb) {
						ret.append("??");
					}
					else {
						ret.append(UCharToStr(pData[n]));
					}
				}
			}
			else {
				ret = GetInsHex(ins);
			}
			return ret;
		}

		//mov [eax+ebx-0x401000],0x401000
		if (ins.ops[1].type == o_imm) {
			unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
			for (char n = 0; n < ins.ops[0].offb; ++n) {
				ret.append(UCharToStr(pData[n]));
			}
			if (IsUserResourceOffset(ins.ops[0].addr)) {
				for (char n = ins.ops[0].offb; n < ins.ops[1].offb; ++n) {
					ret.append("??");
				}
			}
			else {
				for (char n = ins.ops[0].offb; n < ins.ops[1].offb; ++n) {
					ret.append(UCharToStr(pData[n]));
				}
			}
			if (IsUserResourceImm(ins.ops[1].value)) {
				for (char n = ins.ops[1].offb; n < ins.size; ++n) {
					ret.append("??");
				}
			}
			else {
				for (char n = ins.ops[1].offb; n < ins.size; ++n) {
					ret.append(UCharToStr(pData[n]));
				}
			}
			return ret;
		}
	}

	msg("[GetSig_FlexDoubleInst]To do...\n");
	return ret;

label_HEX_PAT:
	if (IsUserResourceImm(ins.ops[1].value)) {
		unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
		for (char n = 0; n < ins.size; ++n) {
			if (n < ins.ops[1].offb) {
				ret.append(UCharToStr(pData[n]));
			}
			else {
				ret.append("??");
			}
		}
	}
	else {
		ret = GetInsHex(ins);
	}
	return ret;
}

qstring ECSigParser::GetSig_Imul(insn_t& ins)
{
	qstring ret;

	if (ins.ops[0].type == o_reg) {
		if (ins.ops[1].type == o_phrase) {
			//imul eax,[eax+ebx]
			if (ins.ops[2].type == o_void) {
				return GetInsHex(ins);
			}
		}
	}

	msg("[GetSig_Imul]To do...\n");
	return ret;
}

qstring ECSigParser::GetSig_LogicInst(insn_t& ins)
{
	qstring ret;

	if (ins.ops[0].type == o_reg) {
		if (ins.ops[1].type == o_reg) {
			ret = GetInsHex(ins);
			return ret;
		}

		//test eax,0x401000
		if (ins.ops[1].type == o_imm) {
			ret = GetInsHex(ins);
			return ret;
		}
	}


	msg("[GetSig_LogicInst]To do...\n");
	return ret;
}


qstring ECSigParser::GetSig_FlexSingleInst(insn_t& ins)
{
	qstring ret;

	//push reg
	if (ins.ops[0].type == o_reg) {
		ret = GetInsHex(ins);
		return ret;
	}

	if (ins.ops[0].type == o_mem) {
		goto label_PAT;
	}

	//push [eax]
	if (ins.ops[0].type == o_phrase) {
		ret = GetInsHex(ins);
		return ret;
	}
	//push [eax+ebx+0x401000]
	if (ins.ops[0].type == o_displ) {
		if (IsUserResourceOffset(ins.ops[0].addr)) {
			goto label_PAT;
		}
		else {
			ret = GetInsHex(ins);
		}
		return ret;
	}
	
	if (ins.ops[0].type == o_imm) {
		if (IsUserResourceImm(ins.ops[0].value)) {
			goto label_PAT;
		}
		else {
			ret = GetInsHex(ins);
		}
		return ret;
	}

	msg("[GetSig_FlexSingleInst]To do...\n");
	return ret;
label_PAT:
	unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(ins.ip);
	for (char n = 0; n < ins.size; ++n) {
		if (n >= ins.ops[0].offb) {
			ret.append("??");
		}
		else {
			ret.append(UCharToStr(pData[n]));
		}
	}
	return ret;
}

void ECSigParser::ScanMSig(const char* lpsigPath, ea_t rangeStart, ea_t rangeEnd)
{
	//mSave_SubFunc.clear();
	//bFuzzySig = false;
	//size_t funcCount = get_func_qty();
	//for (unsigned int idx = 0; idx < funcCount; ++idx)
	//{
	//	func_t* pFunc = getn_func(idx);

	//	if (pFunc->start_ea < rangeStart || pFunc->start_ea >= rangeEnd) {
	//		continue;
	//	}

	//	if (pFunc->start_ea == 0x4A50CF) {
	//		int a = 0;
	//	}

	//	qstring goodMD5 = GetFunctionMD5(pFunc->start_ea);
	//	auto funcCount = map_MSig.count(goodMD5);

	//	if (funcCount == 1) {
	//		auto it = map_MSig.find(goodMD5);
	//		//setFuncName(pFunc->start_ea, it->second.c_str(), SN_FORCE);
	//		msg("%s%a--%s\n", getUTF8String("识别模块函数").c_str(), pFunc->start_ea, getUTF8String(it->second.c_str()).c_str());
	//		continue;
	//	}
	//	else if (funcCount != 0) {
	//		auto it = map_MSig.find(goodMD5);
	//		//setFuncName(pFunc->start_ea, it->second.c_str());
	//		msg("%s%a--%s\n", getUTF8String("识别模块函数").c_str(), pFunc->start_ea, getUTF8String(it->second.c_str()).c_str());
	//		continue;
	//	}
	//	
	//}

	//mSave_SubFunc.clear();
	//funcCount = get_func_qty();
	//bFuzzySig = true;
	//for (unsigned int idx = 0; idx < funcCount; ++idx)
	//{
	//	func_t* pFunc = getn_func(idx);

	//	if (pFunc->start_ea < rangeStart || pFunc->start_ea >= rangeEnd) {
	//		continue;
	//	}

	//	qstring badMD5 = GetFunctionMD5(pFunc->start_ea);
	//	funcCount = map_MSig.count(badMD5);
	//	if (funcCount) {
	//		auto it = map_MSig.find(badMD5);
	//		//setFuncName(pFunc->start_ea, it->second.c_str());
	//		msg("%s%a--%s\n", getUTF8String("识别模块函数").c_str(), pFunc->start_ea, getUTF8String(it->second.c_str()).c_str());
	//		continue;
	//	}
	//	if (funcCount != 0) {

	//	}
	//}
	//qfclose(hFile);
	return;
}

qstring ECSigParser::GetFunctionMD5(ea_t FuncStartAddr)
{
	qstring ret_MD5;
	ea_t startAddr = pFunc->start_ea;
	qvector<qstring> vec_SaveSig;
	bool bSkipNextIns = false;
	do
	{
		qstring tmpSig;
		insn_t CurrentIns;
		int insLen = decode_insn(&CurrentIns, startAddr);
		if (!insLen) {
			
			continue;
		}

#ifdef _DEBUG
		if (CurrentIns.ip == 0x0040204B) {
			int a = 0;
		}
#endif
		if (bSkipNextIns) {
			startAddr = startAddr + CurrentIns.size;
			bSkipNextIns = false;
			continue;
		}
		switch (CurrentIns.itype)
		{
		case NN_ja:
		case NN_jae:
		case NN_jb:
		case NN_jbe:
		case NN_jc:
		case NN_jcxz:
		case NN_jecxz:
		case NN_jrcxz:
		case NN_je:
		case NN_jg:
		case NN_jge:
		case NN_jl:
		case NN_jle:
		case NN_jna:
		case NN_jnae:
		case NN_jnb:
		case NN_jnbe:
		case NN_jnc:
		case NN_jne:
		case NN_jng:
		case NN_jnge:
		case NN_jnl:
		case NN_jnle:
		case NN_jno:
		case NN_jnp:
		case NN_jns:
		case NN_jnz:
		case NN_jo:
		case NN_jp:
		case NN_jpe:
		case NN_jpo:
		case NN_js:
		case NN_jz:
		case NN_jmpfi:
		case NN_jmpni:
		case NN_jmpshort:
			tmpSig = GetInsHex(CurrentIns);
			break;
		case NN_jmp:
			tmpSig = GetSig_LongJmp(CurrentIns);
			break;
		case NN_add:
		case NN_cmp:
		case NN_mov:
		case NN_sub:
			tmpSig = GetSig_FlexDoubleInst(CurrentIns);
			break;
		case NN_fldcw:
		case NN_fnstcw:
		case NN_fstsw:
			tmpSig = GetSig_FloatInstA(CurrentIns);
			break;
		case NN_fld:
		case NN_fstp:
		case NN_fild:
		case NN_fadd:
		case NN_fsub:
		case NN_fmul:
		case NN_fdiv:
		case NN_fcomp:
		case NN_fistp:
		case NN_fstcw:
			tmpSig = GetSig_FloatInstB(CurrentIns);
			break;
		case NN_inc:
		case NN_dec:
		case NN_push:
			tmpSig = GetSig_FlexSingleInst(CurrentIns);
			break;
		case NN_call:
			tmpSig = GetSig_Call(CurrentIns, vec_SaveSig, bSkipNextIns);
			break;
		//nop指令的处理是个例外,这个是预留给易语言的花指令的
		case NN_nop:
			tmpSig = "";
			break;
		//非易语言指令(观察中)
		case NN_lea:
		case NN_setnz:
		case NN_setz:
		case NN_imul:
		case NN_and:
		case NN_or:
		case NN_shl:
		case NN_shr:
		case NN_xor:
		case NN_test:
		case NN_loop:
		case NN_loopd:
		case NN_fninit:
		case NN_cld:
		case NN_sbb:
		case NN_not:
		case NN_mul:
		case NN_ftst:
		case NN_fnstsw:
		case NN_fchs:
		case NN_cwde:
		case NN_movs:
		case NN_pop:
		case NN_retf:
			tmpSig = GetInsHex(CurrentIns);
			break;
			//非易语言指令(高度确认)
		case NN_aaa:
		case NN_aad:
		case NN_aam:
		case NN_aas:
		case NN_adc:
		case NN_arpl:
		case NN_bound:
		case NN_bsf:
		case NN_bsr:
		case NN_bt:
		case NN_btc:
		case NN_btr:
		case NN_bts:
		case NN_cbw:
		case NN_cdqe:
		case NN_clc:
		case NN_cli:
		case NN_clts:
		case NN_cmc:
		case NN_daa:
		case NN_das:
		case NN_wait:
		case NN_cmova:
		case NN_cmovnz:
		case NN_cpuid:
		case NN_leave:
		case NN_xgetbv:
		case NN_vxorpd:
		case NN_vpcmpeqb:
		case NN_vpmovmskb:
		case NN_movdqa:
		case NN_pxor:
		case NN_movq:
		case NN_pcmpeqb:
		case NN_pmovmskb:
		case NN_movzx:
		case NN_vmovdqu:
			tmpSig = GetInsHex(CurrentIns);
			break;
		//无需分析的指令
		case NN_lods:
		case NN_stos:
		case NN_retn:
			tmpSig = GetInsHex(CurrentIns);
			break;
		default:
			//msg("UnHandled Instruction--%a\n", CurrentIns.ip);
			tmpSig = GetInsHex(CurrentIns);
			break;
		}

		vec_SaveSig.push_back(tmpSig);
		if (tmpSig.empty() && CurrentIns.itype != NN_nop) {
			msg("%s--%a\n", getUTF8String("获取特征失败").c_str(), startAddr);
		}
		startAddr = startAddr + CurrentIns.size;

	} while (startAddr <= endAddr);

	qstring STRING_RESULT;
	for (unsigned int n = 0; n < vec_SaveSig.size(); ++n) {
		STRING_RESULT.append(vec_SaveSig[n]);
	}

	//ret_MD5 = CalculateMD5(STRING_RESULT);

#ifdef _DEBUG
	if (!bFuzzySig) {
		msg("[%a]:%s\n", FuncStartAddr, STRING_RESULT.c_str());
	}
#endif // _DEBUG
	
	return ret_MD5;
}