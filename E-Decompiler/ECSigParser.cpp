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

ea_t ECSigParser::SeachEFuncEnd(func_t* pFunc)
{
	ea_t ret = BADADDR;

	compiled_binpat_vec_t binPat;
	parse_binpat_str(&binPat, 0x0, "8B E5 5D", 16);
	segment_t* pSegment = getseg(pFunc->start_ea);
	if (!pSegment) {
		return BADADDR;
	}

	ea_t startAddr = pFunc->start_ea;

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
		if (ret + tmpIns.size >= pFunc->end_ea && tmpIns.itype == NN_retn) {
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

void ECSigParser::InitECSigResource(uint32 startAddr, uint32 endAddr)
{
	m_UserResourceStartAddr = startAddr;
	m_UserResourceEndAddr = endAddr;
}

void ECSigParser::InitECSigBasciFunc(std::map<ea_t, qstring>& mhash)
{
	mMap_BasicFunc.clear();
	mMap_BasicFunc = mhash;
}

void ECSigParser::InitECSigKrnl(eSymbol_KrnlJmp& inFunc)
{
	m_KrnlJmp = inFunc;
}

void ECSigParser::Debug_outputECSig()
{
	ea_t funcStartAddr = 0x401004;

	const char* functionList[] = {
"__get",
"__get_byte",
"__query_bit",
"__set",
"__set_bit_off",
"__set_bit_on",
"__set_byte",
"__set_short",
"_只取汉字",
"AES_DES_算法",
"API_取空白文本",
"API_取空白字节集",
"BASE64编解码_ASM_初始化",
"BASE64编码_ASM",
"BASE64解码_ASM",
"bin2hex",
"CallObject",
"COM_AddRef",
"COM_bstr_t",
"COM_QueryInterface",
"COM_Release",
"COM_StringToCLSID",
"COM_StringtoIID",
"COM_调用COM方法",
"COM_连接命名空间",
"E数据库_快速查询",
"E数据库_修复易数据库",
"hex2bin",
"HIBYTE",
"HIWORD",
"IP_10进制转IP",
"IP_16进制转IP",
"IP_转至10进制",
"IP_转至16进制",
"LOBYTE",
"LOWORD",
"OCX_卸载",
"OCX_注册",
"rar_测试文件",
"RAR_测试压缩文件",
"rar_初始化",
"rar_解压",
"RAR_解压文件",
"rar_取压缩包内的文件数量",
"rar_删除压缩包内的文件",
"rar_添加文件注释",
"rar_压缩",
"SafeRelease",
"编码_Ansi到ASCII_文本型",
"编码_Ansi到Unicode",
"编码_Ansi到Unicode_JS",
"编码_Ansi到Unicode_文本型",
"编码_ansi到usc2",
"编码_ansi到usc2_EX",
"编码_Ansi到Utf8",
"编码_Ansi到Utf8_对象",
"编码_Ansi到Utf8_无BOM文件",
"编码_ASCII到Unicode",
"编码_BASE64编码",
"编码_BASE64编码A",
"编码_BASE64解码",
"编码_BASE64解码A",
"编码_Base64转图片",
"编码_BIG5到繁体",
"编码_BIG5到简体",
"编码_gb2312到utf8",
"编码_RC4加密",
"编码_RC4解密",
"编码_Unicode到Ansi",
"编码_Unicode到Ansi_JS",
"编码_Unicode到Ansi_对象",
"编码_Unicode到Ansi_文本型",
"编码_Unicode到usc2",
"编码_Unicode到Utf8",
"编码_URL编码",
"编码_URL编码_JS",
"编码_URL编码_局部",
"编码_URL编码_快速",
"编码_URL解码",
"编码_URL解码_JS",
"编码_URL解码_局部",
"编码_URL解码_快速",
"编码_usc2到ansi",
"编码_usc2到ansi_EX",
"编码_usc2到Unicode",
"编码_UTF8编码",
"编码_Utf8到Ansi",
"编码_Utf8到Ansi_对象",
"编码_utf8到gb2312",
"编码_Utf8到Unicode",
"编码_UTF8解码",
"编码_U编码转换",
"编码_编码转换对象",
"编码_繁体到BIG5",
"编码_繁体到简体",
"编码_简体到BIG5",
"编码_简体到繁体",
"编码_运行JS代码",
"菜单_点击",
"菜单_点击按标题",
"菜单_枚举",
"菜单_枚举子级菜单",
"菜单_清除",
"菜单_取ID",
"菜单_取标题",
"菜单_取句柄",
"菜单_取数量",
"菜单_取子句柄",
"菜单_删除",
"菜单_添加",
"菜单_添加图标",
"菜单_修改标题",
"菜单_重画",
"程序_Call",
"程序_COM生成GUID",
"程序_标准输出",
"程序_标准输入",
"程序_出错退出",
"程序_等待窗口出现",
"程序_等待窗口消失",
"程序_等待句柄消失",
"程序_等待控件内容改变",
"程序_调用DLL命令",
"程序_更新",
"程序_加入IE工具栏",
"程序_加入右键菜单",
"程序_禁止重复运行",
"程序_内存中运行EXE",
"程序_取安装目录",
"程序_取参数个数",
"程序_取函数地址_API",
"程序_取命令行",
"程序_取文件版本号",
"程序_取子程序地址",
"程序_任务栏窗口图标状态",
"程序_删除自身",
"程序_删除自身1",
"程序_生成GUID",
"程序_是否被调试",
"程序_是否关闭",
"程序_是否在虚拟机中运行",
"程序_锁定到任务栏",
"程序_添加到IE工具栏",
"程序_条件判断",
"程序_显示任务栏图标",
"程序_显示托盘图标",
"程序_写日志",
"程序_延时",
"程序_隐藏任务栏图标",
"程序_隐藏托盘图标",
"程序_运行Ex",
"程序_执行文本子程序",
"程序_执行整数子程序",
"程序_执行整数子程序1",
"程序_重启",
"窗口_按类名枚举",
"窗口_按钮解除禁止",
"窗口_撤销窗口热键",
"窗口_创建Flash",
"窗口_等待出现",
"窗口_发送拖放消息",
"窗口_复制到剪贴板",
"窗口_更换鼠标指针",
"窗口_更新窗口",
"窗口_挂起",
"窗口_关闭",
"窗口_画矩形",
"窗口_还原",
"窗口_激活",
"窗口_监视热键_卸载",
"窗口_监视热键_注册",
"窗口_监视锁屏_卸载",
"窗口_监视锁屏_注册",
"窗口_渐隐渐现",
"窗口_将焦点切换到指定的窗口",
"窗口_禁止关闭",
"窗口_禁止截图",
"窗口_句柄枚举进程信息",
"窗口_句柄取进程ID",
"窗口_句柄取进程路径",
"窗口_句柄取进程名",
"窗口_句柄取线程ID",
"窗口_句柄取线程句柄",
"窗口_句柄是否有效",
"窗口_控件ID取句柄",
"窗口_控件调整_创建",
"窗口_控件调整_销毁",
"窗口_控件加边框",
"窗口_毛玻璃特效",
"窗口_枚举",
"窗口_枚举接口",
"窗口_枚举所有子窗口",
"窗口_枚举子窗口",
"窗口_模糊遍历窗口",
"窗口_屏蔽编辑框右键",
"窗口_屏蔽控件右键",
"窗口_屏蔽浏览器右键",
"窗口_嵌入桌面",
"窗口_强制显示",
"窗口_取IE浏览框句柄",
"窗口_取边框高度",
"窗口_取边框宽度",
"窗口_取标题",
"窗口_取标题W",
"窗口_取窗口属性",
"窗口_取点标题",
"窗口_取顶端窗口句柄",
"窗口_取顶端文件夹路径",
"窗口_取父句柄",
"窗口_取父句柄EX",
"窗口_取光标位置",
"窗口_取光标位置1",
"窗口_取光标坐标",
"窗口_取滚动条位置",
"窗口_取激活句柄",
"窗口_取焦点句柄",
"窗口_取矩形",
"窗口_取句柄",
"窗口_取句柄_递归",
"窗口_取句柄_模糊",
"窗口_取客户区矩形",
"窗口_取控件ID",
"窗口_取控件大小",
"窗口_取控件内容",
"窗口_取控件坐标",
"窗口_取快速启动栏句柄",
"窗口_取扩展样式",
"窗口_取类名",
"窗口_取屏幕句柄",
"窗口_取任务栏句柄",
"窗口_取鼠标处窗口句柄",
"窗口_取鼠标处控件标题",
"窗口_取位置和大小",
"窗口_取消尺寸限制",
"窗口_取小图标",
"窗口_取样式",
"窗口_取指定坐标处句柄",
"窗口_取桌面句柄",
"窗口_取字符串宽度",
"窗口_取祖句柄",
"窗口_取坐标",
"窗口_取坐标处颜色",
"窗口_取坐标距离",
"窗口_取坐标子句柄",
"窗口_热键卸载",
"窗口_热键卸载所有",
"窗口_热键注册",
"窗口_闪动",
"窗口_闪动Ex",
"窗口_设为无焦点",
"窗口_是否被遮挡",
"窗口_是否存在",
"窗口_是否激活",
"窗口_是否禁止",
"窗口_是否可见",
"窗口_是否全屏",
"窗口_是否响应",
"窗口_是否有子窗口",
"窗口_是否在屏幕内",
"窗口_是否在最顶端",
"窗口_是否置顶",
"窗口_是否子窗口",
"窗口_是否最大化",
"窗口_是否最小化",
"窗口_锁住解锁",
"窗口_剔除风格",
"窗口_剔除扩展风格",
"窗口_显示隐藏",
"窗口_限制尺寸",
"窗口_限制运行次数",
"窗口_写字",
"窗口_循环关闭",
"窗口_隐藏任务按钮",
"窗口_隐藏小图标",
"窗口_圆角化",
"窗口_置标题",
"窗口_置穿透",
"窗口_置窗口化",
"窗口_置窗口属性",
"窗口_置顶",
"窗口_置父",
"窗口_置父彗星",
"窗口_置滚动条位置",
"窗口_置焦点",
"窗口_置控件焦点",
"窗口_置控件内容",
"窗口_置控件状态",
"窗口_置透明度",
"窗口_置透明颜色",
"窗口_置位置和大小",
"窗口_置状态",
"窗口_重画",
"窗口_注册窗口热键",
"窗口_总在最前",
"窗口_最大化",
"窗口_最小化",
"创建图标自字节集",
"磁盘_取驱动器总线类型",
"到逻辑型",
"到逻辑型_通用版",
"地址_取API地址",
"调用子程序_",
"调用子程序_数组",
"短整数_有符号转无符号",
"对称加密",
"对称解密",
"对话框_打开文件",
"对话框_打开文件多选框",
"对话框_打开颜色选择框",
"对话框_另存文件",
"对话框_添加到收藏夹",
"对话框_整理收藏夹",
"发送消息逻辑",
"发送消息文本",
"发送消息整数",
"格式化文本A",
"格式化文本A_数组",
"格式化文本W",
"格式化文本W_数组",
"画板_去背景色",
"汇编_取随机数",
"汇编_取随机数_硬件版",
"汇编_取随机数种子_硬件版",
"剪辑板_取数据权窗口句柄",
"剪辑板_取所有内容",
"剪辑板_取图片",
"剪辑板_取位图数据A",
"剪辑板_取位图数据B",
"剪辑板_取文本",
"剪辑板_取文本W",
"剪辑板_取文件",
"剪辑板_置内容",
"剪辑板_置图片",
"剪辑板_置文本",
"剪辑板_置文件",
"剪辑板_置字节集",
"剪贴板_监听",
"剪贴板_停止监听",
"键盘_单击",
"键盘_键代码取键名",
"键盘_键代码取键名1",
"键盘_键名取键代码",
"键盘_解锁",
"键盘_模拟输入",
"键盘_取大小写状态",
"键盘_取功能键名",
"键盘_取功能键状态",
"键盘_取小键盘状态",
"键盘_锁定",
"键盘_消息",
"键盘_组合按键",
"键鼠_锁定",
"进程_DEP保护",
"进程_ID取窗口句柄",
"进程_ID取窗口句柄EX",
"进程_ID取进程名",
"进程_ID取模块",
"进程_ID是否有效",
"进程_PID取端口",
"进程_PID取路径",
"进程_创建",
"进程_创建Ex",
"进程_创建W",
"进程_打开",
"进程_打开1",
"进程_定位",
"进程_端口取PID",
"进程_关闭",
"进程_结束",
"进程_结束1",
"进程_禁止被打开",
"进程_枚举",
"进程_名取ID",
"进程_名取句柄",
"进程_内存使用",
"进程_强力打开进程",
"进程_强力结束进程",
"进程_取ID数组",
"进程_取IO读取计数",
"进程_取IO读取字节",
"进程_取IO其它计数",
"进程_取IO其它字节",
"进程_取IO写入计数",
"进程_取IO写入字节",
"进程_取程序路径",
"进程_取父ID",
"进程_取进程启动时间",
"进程_取句柄数",
"进程_取路径",
"进程_取路径W",
"进程_取命令行",
"进程_取命令行32",
"进程_取模块句柄",
"进程_取启动参数",
"进程_取同名ID",
"进程_取同名IDW",
"进程_取系统进程列表",
"进程_取线程数",
"进程_取用户名",
"进程_取优先级",
"进程_取远程会话ID",
"进程_取子进程ID",
"进程_取子进程IDW",
"进程_取自进程ID",
"进程_是否被挂起",
"进程_是否存在",
"进程_是否为64位",
"进程_守护",
"进程_守护W",
"进程_提升权限",
"进程_提升权限到Debug",
"进程_暂停",
"进程_置优先级",
"进程_终止进程",
"进程_终止进程W",
"进程通信_发送端_发送数据",
"进程通信_接收端_初始化",
"进程通信_接收端_关闭监听",
"进程通信_接收端_开始监听",
"进程通信_接收端_清除数据",
"进程通信_接收端_清空数据",
"进程通信_接收端_取出数据",
"进程通信_接收端_停止监听",
"进度条_改颜色",
"进度条_滚动",
"进度条_模拟",
"进度条_模拟S",
"进制_八到十",
"进制_二八十六到十",
"进制_二到十",
"进制_三十六到十",
"进制_十到八",
"进制_十到二",
"进制_十到三十六",
"进制_十到十六",
"进制_十六到十",
"进制_十六到十_汇编",
"进制_十六进制转有符号的十进制",
"进制_有符号的十进制转十六进制",
"控件_圆角化",
"控制台_快速编辑模式",
"类_取内部方法地址",
"类_释放内部方法地址",
"类回调_取类地址",
"类回调_释放回调",
"列表框_保存为配置文件",
"列表框_读入配置文件",
"目录_处理",
"目录_创建",
"目录_创建W",
"目录_打开虚拟文件夹",
"目录_定位",
"目录_复制",
"目录_复制文件_cmd",
"目录_改名",
"目录_更新",
"目录_检查系统属性",
"目录_枚举子目录",
"目录_枚举子目录1",
"目录_强力清空",
"目录_清空",
"目录_取system32目录",
"目录_取Windows目录",
"目录_取x86系统目录",
"目录_取创建时间",
"目录_取大小",
"目录_取当前目录",
"目录_取当前目录W",
"目录_取父路径",
"目录_取临时目录",
"目录_取特定目录",
"目录_取特定目录EX",
"目录_取特定目录W",
"目录_取尾部目录名或文件名",
"目录_取文件数",
"目录_取消系统属性",
"目录_取运行目录",
"目录_取运行目录W",
"目录_取桌面目录",
"目录_取子目录数",
"目录_删除目录",
"目录_删除目录W",
"目录_删除自身_cmd",
"目录_设置系统属性",
"目录_是否存在",
"目录_是否存在W",
"目录_是否符合规范",
"目录_是否为空",
"目录_是否有子目录",
"目录_同步更新",
"目录_移动并运行_cmd",
"目录_移动文件_cmd",
"目录_浏览",
"内部_读入字节集",
"内部_取文件长度",
"内存_拷贝字节集",
"内存_取calljmp地址",
"内存_取指令对其长度",
"内存_申请内存",
"内存_释放远程内存",
"内存_跳转偏移处理",
"内存_写到内存",
"内存_修复堆内存重释放",
"内存_修改内存",
"内存_远程创建内存_文本",
"内存_远程创建内存_字节集",
"内存_远程调用子程序",
"批量_编辑框_保存配置",
"批量_编辑框_读取配置",
"批量_单选框_保存配置",
"批量_单选框_读取配置",
"批量_日期框_保存配置",
"批量_日期框_读取配置",
"批量_选择框_保存配置",
"批量_选择框_读取配置",
"批量_组合框_保存配置",
"批量_组合框_读取配置",
"屏幕亮度_初始化",
"屏幕亮度_调整",
"屏幕亮度_销毁",
"取变量堆栈地址_字节集",
"取变体型指针",
"取错误信息文本_API",
"取对象指针",
"取数据_通用型",
"取数据_通用型_数组",
"取文本长度_ASM",
"取指针_通用型",
"取指针_通用型_数组",
"取指针_文本型",
"取指针_字节集型",
"取指针地址_长整数型",
"取指针地址_短整数型",
"取指针地址_文本型",
"取指针地址_小数型",
"取指针地址_整数型",
"取指针地址_子程序指针",
"取指针地址_字节集型",
"取指针地址_字节型",
"取指针双精度_",
"取指针文本_",
"取指针小数_",
"取指针整数_",
"取指针字节集_",
"取子程序字节集代码",
"删除Unicode结尾空白字节",
"时间_GMT转为时间",
"时间_GMT转为时间1",
"时间_JSON转为时间",
"时间_WMI时间转日期",
"时间_北京转格林威治",
"时间_北京转十八位时间戳",
"时间_到时间戳",
"时间_到文本",
"时间_格林威治转北京",
"时间_格式化",
"时间_格式化EX",
"时间_秒到时分秒格式",
"时间_判断二十四节气",
"时间_取北京时间",
"时间_取北京时间戳",
"时间_取第几周",
"时间_取公历节日",
"时间_取间隔_结束",
"时间_取间隔_开始",
"时间_取节气文本",
"时间_取美式日期",
"时间_取某年天数",
"时间_取农历节日",
"时间_取身份证比较",
"时间_取身份证判断",
"时间_取身份证日期",
"时间_取身份证性别",
"时间_取时间间隔",
"时间_取随机时间戳",
"时间_取现行时间戳",
"时间_取现行时间戳1",
"时间_取月初",
"时间_取月份",
"时间_取月末",
"时间_取月总天数",
"时间_取制式",
"时间_取中国星期几",
"时间_身份证号15到18",
"时间_身份证号转换",
"时间_十八位时间戳转北京",
"时间_时间戳转文本",
"时间_时间转为JSON",
"时间_同步校时",
"时间_星期几取日期",
"时间_转为GMT格式",
"时间_转为GMT格式1",
"时间_转为GMT格式2",
"时间_转为GMT格式3",
"时钟_创建",
"时钟_销毁",
"鼠标_按键",
"鼠标_捕获",
"鼠标_带轨迹移动",
"鼠标_单击",
"鼠标_改指针",
"鼠标_归位",
"鼠标_记位",
"鼠标_监视_安装",
"鼠标_监视_卸载",
"鼠标_解锁",
"鼠标_取窗口坐标",
"鼠标_取双击间隔时间",
"鼠标_取位置",
"鼠标_删除鼠标移入移出事件",
"鼠标_删除所有鼠标移入移出事件",
"鼠标_释放",
"鼠标_锁定",
"鼠标_添加鼠标移入移出事件",
"鼠标_显示隐藏",
"鼠标_限制",
"鼠标_消息",
"鼠标_移动",
"鼠标_左右键交换",
"数组_反转",
"数组_合并",
"数组_合并_快速",
"数组_排序",
"数组_取次数",
"数组_去重复",
"数组_去重复_整数型",
"数组_输出",
"数组_寻找数组",
"提示框",
"投递消息整数",
"图标_取句柄",
"图标_取数量",
"图片_到24位图",
"图片_分割",
"图片_高斯模糊",
"图片_后台截图",
"图片_后台区域截图",
"图片_加水印",
"图片_拼接",
"图片_拼接1",
"图片_平铺拉伸",
"图片_屏幕截图",
"图片_屏幕区域截图",
"图片_取高度",
"图片_取格式",
"图片_取格式W",
"图片_取句柄",
"图片_取宽度",
"图片_取图片区域",
"图片_取像素RGB",
"图片_取照片拍摄日期",
"图片_缩放",
"图片_缩放_透明",
"图片_通过句柄取图标",
"图片_置像素RGB",
"图片_转换",
"图片_转换1",
"图片_转换Ex",
"图片_转换Ex_取最后错误",
"外部编辑框_撤消",
"外部编辑框_复制",
"外部编辑框_滚到最后行",
"外部编辑框_滚动行",
"外部编辑框_加入文本",
"外部编辑框_剪切",
"外部编辑框_内容是否改变过",
"外部编辑框_取光标所在行和列号",
"外部编辑框_取光标所在行和列号W",
"外部编辑框_取光标位置",
"外部编辑框_取密码框文本",
"外部编辑框_取内容",
"外部编辑框_取消选择",
"外部编辑框_取行数",
"外部编辑框_取指定行文本",
"外部编辑框_取指定行文本长度",
"外部编辑框_全选",
"外部编辑框_删除",
"外部编辑框_设为密码方式",
"外部编辑框_设为只读方式",
"外部编辑框_替换内容",
"外部编辑框_限制字符数",
"外部编辑框_选中按内容",
"外部编辑框_选中按位置",
"外部编辑框_在光标处加入文本",
"外部编辑框_置内容",
"外部编辑框_置起始选择位置",
"外部编辑框_置四边距",
"外部编辑框_置左右边距",
"外部编辑框_转成IE地址框",
"外部超级列表框_保存数据",
"外部超级列表框_保存数据_NEW",
"外部超级列表框_取鼠标处列坐标",
"外部超级列表框_取鼠标处行列号",
"外部超级列表框_上下移动",
"外部超级列表框_上移",
"外部超级列表框_下移",
"外部超级列表框_自动调整列宽",
"外部单选框_取消选中",
"外部单选框_取状态",
"外部单选框_选中",
"外部滚动条_显示隐藏",
"外部列表框_保存数据",
"外部列表框_插入项目",
"外部列表框_查找表项",
"外部列表框_加入项目",
"外部列表框_清空",
"外部列表框_取鼠标处项目索引",
"外部列表框_取项目数",
"外部列表框_取项目数值",
"外部列表框_取项目文本",
"外部列表框_取选中项",
"外部列表框_删除项目",
"外部列表框_鼠标坐标取项目索引",
"外部列表框_置项目数值",
"外部列表框_置选中项",
"外部选择框_点击",
"外部选择框_取消选中",
"外部选择框_取状态",
"外部选择框_选中",
"外部选择列表框_插入项目",
"外部选择列表框_打勾",
"外部选择列表框_加入项目",
"外部选择列表框_清空",
"外部选择列表框_取项目数",
"外部选择列表框_取项目数值",
"外部选择列表框_取项目文本",
"外部选择列表框_选中",
"外部选择列表框_置项目数值",
"外部组合框_保存数据",
"外部组合框_插入项目",
"外部组合框_查找",
"外部组合框_弹开列表",
"外部组合框_关闭列表",
"外部组合框_加入项目",
"外部组合框_精确查找",
"外部组合框_清空",
"外部组合框_取高度",
"外部组合框_取结构信息",
"外部组合框_取下拉高度",
"外部组合框_取下拉宽度",
"外部组合框_取项目数",
"外部组合框_取项目数值",
"外部组合框_取项目文本",
"外部组合框_取选中项",
"外部组合框_删除项目",
"外部组合框_设置高度",
"外部组合框_设置下拉高度",
"外部组合框_设置下拉宽度",
"外部组合框_向上展开列表",
"外部组合框_置现行选中项",
"外部组合框_置项目数值",
"网络_取网络文件大小_对象版",
"网页_Cookie合并更新",
"网页_Cookie合并更新ex",
"网页_eval加密",
"网页_eval解密",
"网页_GZIP解压",
"网页_GZIP解压_文本",
"网页_GZIP压缩",
"网页_GZIP压缩_文本",
"网页_IE浏览器网页跳转静音",
"网页_JS格式化",
"网页_JS格式化_EX",
"网页_URL取本地地址",
"网页_保存",
"网页_保存html文档",
"网页_参数Ascii排序",
"网页_查看网页属性",
"网页_处理协议头",
"网页_处理协议头ex",
"网页_创建快捷方式",
"网页_打开指定网址",
"网页_打印",
"网页_打印预览",
"网页_访问",
"网页_访问_对象",
"网页_访问S",
"网页_更改IE版本",
"网页_还原网页颜色",
"网页_禁止允许gif图片",
"网页_禁止允许背景声音",
"网页_禁止允许点击声音",
"网页_禁止允许显示图片",
"网页_禁止允许一键操作",
"网页_开始屏蔽信息框",
"网页_屏蔽网页复制提示",
"网页_清除Cookie",
"网页_清除Cookie文件",
"网页_取Cookie",
"网页_取Cookie2",
"网页_取IP地址",
"网页_取编码",
"网页_取单条Cookie",
"网页_取端口",
"网页_取快捷方式地址",
"网页_取十进制颜色值",
"网页_取外网IP",
"网页_取网络文件尺寸",
"网页_取网络文件名",
"网页_取网页源码",
"网页_取网址文件名",
"网页_取网址引用",
"网页_取文本_dom",
"网页_取文本_reg",
"网页_取页面地址",
"网页_取域名",
"网页_全选",
"网页_删除IE缓存文件",
"网页_设置网页字体",
"网页_是否为网页",
"网页_停止屏蔽信息框",
"网页_网络文件是否存在",
"网页_文字翻译",
"网页_协议头_取信息",
"网页_掩码计算",
"网页_掩码取结束IP",
"网页_置Cookie",
"网页_置Cookie1",
"网页_置浏览器UA",
"网页_置浏览器代理",
"网页_转换为HTML色",
"网页_自动点击网页信息框",
"网页_自动点击网页信息框_带返回值",
"网址_取所有参数",
"网址_取指定参数值",
"文本_插入文本到关键字所在行后",
"文本_插入文本到某位置",
"文本_插入文本到某行后",
"文本_插入文本到某行前",
"文本_插入文本到某字后",
"文本_插入文本到某字前",
"文本_插入新一行",
"文本_倒取出中间文本",
"文本_倒取中间_批量",
"文本_到大写",
"文本_到小写",
"文本_到小写EX",
"文本_到小写m",
"文本_递减",
"文本_递增",
"文本_颠倒",
"文本_发送",
"文本_分割文本",
"文本_加密",
"文本_加密_优化",
"文本_加密c",
"文本_解密",
"文本_解密_优化",
"文本_解密c",
"文本_均分文本",
"文本_均分文本1",
"文本_朗读",
"文本_朗读Ex",
"文本_取GB2312简体汉字笔画数",
"文本_取长度",
"文本_取长度ex",
"文本_取长度W",
"文本_取出N组不重复数字",
"文本_取出文本中汉字",
"文本_取出现次数",
"文本_取出中间文本",
"文本_取汉字机内码",
"文本_取汉字区位码",
"文本_取汉字区位码2",
"文本_取空白文本",
"文本_取某位置文本",
"文本_取全部汉字拼音",
"文本_取声母",
"文本_取首拼",
"文本_取随机Ip",
"文本_取随机范围数字",
"文本_取随机汉字",
"文本_取随机汉字_快速",
"文本_取随机数字",
"文本_取随机姓氏",
"文本_取随机字符",
"文本_取随机字母",
"文本_取文本起始位置",
"文本_取文本所在行",
"文本_取文本所在行_优化版",
"文本_取文本行出现次数",
"文本_取文本行起始位置",
"文本_取行数",
"文本_取右边",
"文本_取指定变量文本行",
"文本_取指定变量文本行1",
"文本_取指定文件文本行",
"文本_取指定文件文本行1",
"文本_取中间_批量",
"文本_取中间_批量_正则方式",
"文本_取字符串宽度",
"文本_取左边",
"文本_去重复文本",
"文本_全选",
"文本_删除空行",
"文本_删除空行_汇编",
"文本_删除指定变量文本行1",
"文本_删除指定文本行",
"文本_删除指定文件文本行1",
"文本_删除指定文件行",
"文本_删首尾空",
"文本_删右边",
"文本_删中间",
"文本_删中间2",
"文本_删左边",
"文本_是否UTF8_汇编",
"文本_是否大写字母",
"文本_是否为汉字",
"文本_是否为汉字2",
"文本_是否为数字",
"文本_是否为双字节字符",
"文本_是否为双字节字符Ex",
"文本_是否为字母",
"文本_是否小写字母",
"文本_是否重复",
"文本_首字母改大写",
"文本_替换",
"文本_替换行内容",
"文本_投递",
"文本_投递W",
"文本_文本比较W",
"文本_文本数组排序",
"文本_寻找出文本并删除",
"文本_寻找文本",
"文本_粘贴",
"文本_折行",
"文本_指针到文本A",
"文本_指针到文本W",
"文本_逐字分割",
"文本_逐字分割_汇编",
"文本_逐字分割_快速",
"文本_逐字分割_快速_释放",
"文本_转拼音",
"文本_自动补零",
"文本区分_只取符号",
"文本区分_只取汉字",
"文本区分_只取数字",
"文本区分_只取字母",
"文件_创建",
"文件_创建文件目录",
"文件_创建稀疏文件",
"文件_创建映射对象",
"文件_打开",
"文件_到长文件名",
"文件_到短文件名",
"文件_到短文件名W",
"文件_定位",
"文件_定位W",
"文件_定位多个",
"文件_定位多个W",
"文件_读取映射对象",
"文件_读入字节集",
"文件_复制",
"文件_复制W",
"文件_改扩展名",
"文件_改名",
"文件_关闭",
"文件_关联",
"文件_合并文件",
"文件_后缀是否关联",
"文件_句柄取路径",
"文件_路径是否规范",
"文件_枚举",
"文件_枚举1",
"文件_枚举EX",
"文件_枚举W",
"文件_取MP3播放时间",
"文件_取尺寸",
"文件_取大小",
"文件_取格式",
"文件_取关联",
"文件_取扩展名",
"文件_取类型",
"文件_取路径盘符",
"文件_取目录",
"文件_取目录W",
"文件_取配置节名数组",
"文件_取配置项名数组",
"文件_取配置项名数组1",
"文件_取时间",
"文件_取时间W",
"文件_取实际大小",
"文件_取属性",
"文件_取图标",
"文件_取图标句柄",
"文件_取文本编码",
"文件_取文件版本号",
"文件_取文件名",
"文件_取文件信息",
"文件_去扩展名",
"文件_删除",
"文件_删除W",
"文件_删除到回收站",
"文件_删除某类文件",
"文件_删除配置节",
"文件_删除配置项",
"文件_是否被占用",
"文件_是否存在",
"文件_是否存在W",
"文件_是否是稀疏文件",
"文件_是否为目录",
"文件_是否为目录W",
"文件_搜索",
"文件_搜索1",
"文件_搜索_深度",
"文件_文件名是否可用",
"文件_下载",
"文件_写出字节集",
"文件_循环删除",
"文件_寻找文件W",
"文件_移除属性",
"文件_移除属性W",
"文件_移动",
"文件_移动W",
"文件_移动读写位置",
"文件_执行",
"文件_执行W",
"文件_置时间",
"文件_置时间W",
"文件_重启电脑后替换",
"系统_IP路由是否启用",
"系统_WINS代理是否启用",
"系统_处理事件",
"系统_处理事件1",
"系统_创建还原点",
"系统_创建桌面快捷方式",
"系统_磁盘是否支持稀疏文件",
"系统_打开控制面板",
"系统_打开控制面板项",
"系统_打开网络连接属性",
"系统_调用帮助",
"系统_格式化软盘",
"系统_关电源",
"系统_关机",
"系统_关联右键菜单带图标",
"系统_光驱查询",
"系统_光驱开关",
"系统_还原三原色",
"系统_恢复电源管理",
"系统_恢复屏保",
"系统_恢复任务管理器",
"系统_恢复任务管理器1",
"系统_恢复文件系统重定向",
"系统_加入域",
"系统_检测host",
"系统_建立关联",
"系统_进入待机状态",
"系统_禁用DPI缩放",
"系统_禁用文件系统重定向",
"系统_开启屏保",
"系统_开始按钮显示",
"系统_开始按钮隐藏",
"系统_临时字体_安装",
"系统_临时字体_卸载",
"系统_判断有无摄像头",
"系统_屏蔽任务管理器",
"系统_屏蔽任务管理器1",
"系统_启用本地连接",
"系统_强制关机",
"系统_强制重启",
"系统_强制注销",
"系统_切换用户",
"系统_清除托盘残留",
"系统_清除托盘残留1",
"系统_清空回收站",
"系统_清空系统临时目录",
"系统_取CPU二级缓存",
"系统_取CPU描述",
"系统_取CPU名称",
"系统_取CPU时钟频率",
"系统_取CPU数据宽度",
"系统_取CPU型号",
"系统_取CPU序列号",
"系统_取CPU一级缓存",
"系统_取CPU占用率",
"系统_取CPU制造商",
"系统_取DOS路径",
"系统_取DOS执行结果",
"系统_取DOS执行结果T",
"系统_取IE版本",
"系统_取IE代理",
"系统_取MAC地址",
"系统_取本机IP",
"系统_取本机名",
"系统_取操作系统类别",
"系统_取磁盘大小信息",
"系统_取磁盘类型",
"系统_取磁盘信息",
"系统_取货币符号",
"系统_取计算机名",
"系统_取计算机语言",
"系统_取空闲时间",
"系统_取宽带线路",
"系统_取宽带用户名密码",
"系统_取屏幕分辨率",
"系统_取屏幕数量",
"系统_取任务栏高度",
"系统_取日期格式",
"系统_取摄像头数量",
"系统_取时间格式",
"系统_取时区",
"系统_取输入设备数量",
"系统_取数值注册项W",
"系统_取所有磁盘盘符",
"系统_取所有时区",
"系统_取所有用户名",
"系统_取网卡信息",
"系统_取网卡信息_彗星版",
"系统_取网络节点类型",
"系统_取网络连接信息",
"系统_取网络连接状态",
"系统_取网络主DNS后缀",
"系统_取网络主机名",
"系统_取文本注册项W",
"系统_取系统DPI",
"系统_取系统DPI_注册表版",
"系统_取系统版本",
"系统_取系统版本_汇编",
"系统_取系统信息",
"系统_取系统信息1",
"系统_取显卡信息列表",
"系统_取消电源管理",
"系统_取星座",
"系统_取硬盘特征字",
"系统_取硬盘特征字1",
"系统_取用户名",
"系统_取远程会话身份名称",
"系统_取远程机器名",
"系统_取运行参数",
"系统_取主页地址",
"系统_取桌面快捷方式",
"系统_取字体名",
"系统_去掉屏保",
"系统_任务窗口显示",
"系统_任务窗口隐藏",
"系统_任务栏显示",
"系统_任务栏隐藏",
"系统_删除MAC地址",
"系统_删除还原点",
"系统_删除计划任务",
"系统_设置IE代理地址",
"系统_设置时区",
"系统_设置随机启动",
"系统_设置为默认浏览器",
"系统_生成随机mac",
"系统_是否64位操作系统",
"系统_是否为管理员",
"系统_是否为默认字体",
"系统_是否已联网",
"系统_输入法选择",
"系统_刷新",
"系统_刷新屏幕",
"系统_添加计划任务",
"系统_通信测试",
"系统_退出域",
"系统_托盘图标显示",
"系统_托盘图标隐藏",
"系统_系统时间显示",
"系统_系统时间隐藏",
"系统_显示关机对话框",
"系统_显示器操作",
"系统_显示托盘",
"系统_显示隐藏快速启动栏",
"系统_显示用户操作区",
"系统_显示桌面",
"系统_写数值注册项W",
"系统_写文本注册项W",
"系统_卸载组件",
"系统_信息框Ex",
"系统_修改DNS",
"系统_修改IP地址和网关和子网掩码",
"系统_修改MAC地址",
"系统_修改开机密码",
"系统_修改用户名",
"系统_修改主页",
"系统_以管理员模式创建进程",
"系统_以管理员模式创建进程W",
"系统_隐藏托盘",
"系统_隐藏用户操作区",
"系统_远程关机",
"系统_运行死慢",
"系统_置本地连接状态",
"系统_置屏幕分辨率",
"系统_置桌面背景",
"系统_重启",
"系统_重启资源管理器",
"系统_主板发声",
"系统_注册项是否存在W",
"系统_注册组件",
"系统_注销",
"系统_转换为IP地址",
"系统_转换为主机名",
"系统_桌面图标显示",
"系统_桌面图标隐藏",
"线程_DLL卸载",
"线程_DLL注入",
"线程_初始化COM库",
"线程_创建许可证",
"线程_打开",
"线程_等待",
"线程_挂起",
"线程_关闭句柄",
"线程_恢复",
"线程_进入许可区",
"线程_枚举",
"线程_启动",
"线程_启动1",
"线程_启动2",
"线程_启动_4字节",
"线程_启动_8字节",
"线程_启动_文本型",
"线程_启动多参_文本型",
"线程_启动多参_整数型",
"线程_取ID",
"线程_取消COM库",
"线程_取中止线程退出代码",
"线程_取状态",
"线程_取自线程ID",
"线程_取自线程句柄",
"线程_删除许可证",
"线程_退出许可区",
"线程_销毁",
"线程_置CPU",
"校验_取crc32",
"校验_取hmac_md5",
"校验_取md2",
"校验_取md4",
"校验_取md5",
"校验_取md5_进度",
"校验_取md5_文本",
"校验_取rc4",
"校验_取sha1",
"校验_取sha256",
"校验_取sha512",
"信号量_创建",
"信号量_打开",
"信号量_递减",
"信号量_销毁",
"信号量_增加",
"颜色_取反色",
"易语言_加快捷方式到IE浏览器",
"易语言_取易语言安装目录",
"易语言_引用易模块",
"音乐_播放",
"音乐_进度",
"音乐_时长",
"音乐_停止",
"音乐_暂停",
"音量_减少",
"音量_静音",
"音量_取声道",
"音量_增加",
"音量_置声道",
"运算_表达式计算",
"运算_大小端转换",
"运算_二分查找",
"运算_函数计算",
"运算_括号计算",
"运算_厘米到象素",
"运算_内部计算",
"运算_求反余弦",
"运算_求反正弦",
"运算_求阶乘",
"运算_求直线距离",
"运算_取圆周率",
"运算_四舍六入五留双",
"运算_象素到厘米",
"运算_颜色转换",
"整数_有符号转无符号",
"整数转浮点数",
"指针_到长整数型_汇编",
"指针_到短整数型_汇编",
"指针_到逻辑型_汇编",
"指针_到日期时间型_汇编",
"指针_到双精度小数型_汇编",
"指针_到文本",
"指针_到小数型_汇编",
"指针_到整数",
"指针_到字节集",
"指针_到字节型_汇编",
"指针_取低16位",
"指针_取高16位",
"指针_是否有效",
"指针到变量",
"指针还原",
"指针转移",
"转换_转数据大小为标准显示",
"自绘窗口_格式化信息",
"自绘窗口_解析",
"自绘窗口_取信息",
"自绘窗口_鼠标点击",
"自绘窗口_投递文本",
"字节集_到文本",
"字节集_到整数",
"字节集_取长度",
"字节集_取空白",
"字节集_取空白字节集",
"字节集_取右边",
"字节集_取指定位置字节集",
"字节集_取指针",
"字节集_取中间",
"字节集_取左边",
"字节集_十六进制到字节集",
"字节集_十六进制到字节集2",
"字节集_替换",
"字节集_文本转字节集",
"字节集_文本转字节集ex",
"字节集_寻找",
"字节集_寻找取右",
"字节集_寻找取中",
"字节集_寻找取左",
"字节集_寻找子",
"字节集_子替换",
"字节集_字节集到十六进制",
"字节集_字节集到十六进制2",
"字节集_字节集转文本",
"字节集_字节集转文本ex"
	};

	func_t* pFunc = get_func(funcStartAddr);
	if (!pFunc) {
		return;
	}

	if (!IsEStandardFunction(pFunc->start_ea)) {
		return;
	}

	ea_t endAddr = SeachEFuncEnd(pFunc);
	if (endAddr == BADADDR) {
		return;
	}

	FILE* hFile = qfopen("D:\\ecoutput.txt", "wb");
	if (!hFile) {
		return;
	}

	int nFuncIndex = 0;
	ea_t startAddr = pFunc->start_ea;
	do
	{
		qstring tmpSig;
		insn_t CurrentIns;
		int insLen = decode_insn(&CurrentIns, startAddr);

		if (!insLen) {
			msg("[Debug_outputECSig]error,decode instruction...\n");
			return;
		}

		if (CurrentIns.itype == NN_call) {
			ea_t callAddr = CurrentIns.ops[0].addr;

			if (callAddr == m_KrnlJmp.Jmp_MReportError) {}
			else if (callAddr == m_KrnlJmp.Jmp_MCallDllCmd) {}
			else if (callAddr == m_KrnlJmp.Jmp_MCallLibCmd) {}
			else if (callAddr == m_KrnlJmp.Jmp_MCallKrnlLibCmd) {}
			else if (callAddr == m_KrnlJmp.Jmp_MMalloc) {}
			else if (callAddr == m_KrnlJmp.Jmp_MRealloc) {}
			else if (callAddr == m_KrnlJmp.Jmp_MFree) {}
			else {
				mSave_SubFunc.clear();
				bFuzzySig = false;
				qstring goodmd5 = GetFunctionMD5(callAddr);
				
				mSave_SubFunc.clear();
				bFuzzySig = true;
				qstring badmd5 = GetFunctionMD5(callAddr);

				qstring funcName = functionList[nFuncIndex];
				
				if (goodmd5 == badmd5) {
					qfprintf(hFile,"%s:%s\n", funcName.c_str(), badmd5.c_str());
				}
				else {
					qfprintf(hFile, "%s:%s\n", funcName.c_str(), goodmd5.c_str());
					qfprintf(hFile, "%s_模糊:%s\n", funcName.c_str(), badmd5.c_str());
				}

				nFuncIndex++;
			}
		}
	

		startAddr = startAddr + CurrentIns.size;

	} while (startAddr <= endAddr);

	qfclose(hFile);
}

void ECSigParser::ScanMSig(const char* lpsigPath, ea_t rangeStart, ea_t rangeEnd)
{
	qstring str_filePath;
	acp_utf8(&str_filePath, lpsigPath);

	FILE* hFile = fopenRB(str_filePath.c_str());
	if (!hFile)
	{
		return;
	}

	std::multimap<qstring, qstring> map_MSig;

	qstring str_Line;
	while (-1 != qgetline(&str_Line, hFile)) {
		size_t spitIndex = str_Line.find(":");
		if (spitIndex == qstring::npos) {
			continue;
		}
		qstring funcName = str_Line.substr(0, spitIndex);
		qstring funcSig = str_Line.substr(spitIndex + 1);
		if (funcSig.last() == '\r') {
			funcSig.remove_last();
		}
		if (funcSig.length() != 32) {
			continue;
		}
		map_MSig.insert(std::make_pair(funcSig, funcName));
	}

	mSave_SubFunc.clear();
	bFuzzySig = false;
	size_t funcCount = get_func_qty();
	for (unsigned int idx = 0; idx < funcCount; ++idx)
	{
		func_t* pFunc = getn_func(idx);

		if (pFunc->start_ea < rangeStart || pFunc->start_ea >= rangeEnd) {
			continue;
		}

		if (pFunc->start_ea == 0x4A50CF) {
			int a = 0;
		}

		qstring goodMD5 = GetFunctionMD5(pFunc->start_ea);
		auto funcCount = map_MSig.count(goodMD5);

		if (funcCount == 1) {
			auto it = map_MSig.find(goodMD5);
			//setFuncName(pFunc->start_ea, it->second.c_str(), SN_FORCE);
			msg("%s%a--%s\n", getUTF8String("识别模块函数").c_str(), pFunc->start_ea, getUTF8String(it->second.c_str()).c_str());
			continue;
		}
		else if (funcCount != 0) {
			auto it = map_MSig.find(goodMD5);
			//setFuncName(pFunc->start_ea, it->second.c_str());
			msg("%s%a--%s\n", getUTF8String("识别模块函数").c_str(), pFunc->start_ea, getUTF8String(it->second.c_str()).c_str());
			continue;
		}
		
	}

	mSave_SubFunc.clear();
	funcCount = get_func_qty();
	bFuzzySig = true;
	for (unsigned int idx = 0; idx < funcCount; ++idx)
	{
		func_t* pFunc = getn_func(idx);

		if (pFunc->start_ea < rangeStart || pFunc->start_ea >= rangeEnd) {
			continue;
		}

		qstring badMD5 = GetFunctionMD5(pFunc->start_ea);
		funcCount = map_MSig.count(badMD5);
		if (funcCount) {
			auto it = map_MSig.find(badMD5);
			//setFuncName(pFunc->start_ea, it->second.c_str());
			msg("%s%a--%s\n", getUTF8String("识别模块函数").c_str(), pFunc->start_ea, getUTF8String(it->second.c_str()).c_str());
			continue;
		}
		if (funcCount != 0) {

		}
	}
	qfclose(hFile);
	return;
}

qstring ECSigParser::GetFunctionMD5(ea_t FuncStartAddr)
{
	qstring ret_MD5;

	func_t* pFunc = get_func(FuncStartAddr);
	if (!pFunc) {
		return ret_MD5;
	}

	if (!IsEStandardFunction(pFunc->start_ea)) {
		return ret_MD5;
	}

	ea_t endAddr = SeachEFuncEnd(pFunc);
	if (endAddr == BADADDR) {
		return ret_MD5;
	}

	ea_t startAddr = pFunc->start_ea;
	qvector<qstring> vec_SaveSig;
	bool bSkipNextIns = false;
	do
	{
		qstring tmpSig;
		insn_t CurrentIns;
		int insLen = decode_insn(&CurrentIns, startAddr);
		
		if (!insLen) {
			ea_t nextCodeAddr = find_code(startAddr, SEARCH_DOWN | SEARCH_NOSHOW);
			if (nextCodeAddr >= endAddr) {
				nextCodeAddr = endAddr;
			}
			unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(startAddr);
			for (unsigned int n = 0; n < nextCodeAddr - startAddr; ++n) {
				tmpSig.append(UCharToStr(pData[n]));
			}
			vec_SaveSig.push_back(tmpSig);
			startAddr = nextCodeAddr;
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

	ret_MD5 = CalculateMD5(STRING_RESULT);


#ifdef _DEBUG
	if (!bFuzzySig) {
		msg("[%a]:%s\n", FuncStartAddr, STRING_RESULT.c_str());
	}
#endif // _DEBUG
	
	return ret_MD5;
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

	mSave_SubFunc.clear();
	bFuzzySig = false;
	qstring GoodMd5 = GetFunctionMD5(pFunc->start_ea);
	
	mSave_SubFunc.clear();
	bFuzzySig = true;
	qstring BadMd5 = GetFunctionMD5(pFunc->start_ea);

	msg("[%s%a]:%s\n", getUTF8String("精确特征").c_str(), pFunc->start_ea, GoodMd5.c_str());
	msg("[%s%a]:%s\n", getUTF8String("模糊特征").c_str(), pFunc->start_ea, BadMd5.c_str());

	return true;
}