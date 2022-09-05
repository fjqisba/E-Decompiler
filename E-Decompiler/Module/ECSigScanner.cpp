#include "ECSigScanner.h"
#include <string>
#include <map>
#include <pro.h>
#include <fpro.h>
#include <diskio.hpp>
#include <funcs.hpp>
#include <ua.hpp>
#include <allins.hpp>
#include <segment.hpp>
#include <search.hpp>
#include "../SectionManager.h"
#include "../Utils/Common.h"
#include "../Utils/IDAWrapper.h"
#include "../ESymbol.h"

ECSigScanner gECSigScanner;

//是否为易语言标准函数

bool isEStandardFunction(unsigned int startAddr)
{
	unsigned int head = get_dword(startAddr);
	if ((head & 0xFFFFFF) == 0xEC8B55) {
		return true;
	}
	return false;
}

//初始化特征库

void initMSigMap(std::multimap<std::string, std::string>& sigMap)
{
	std::string sigDir = IDAWrapper::idadir("plugins");
	sigDir.append("\\esig");

	std::vector<std::string> sigFileList = IDAWrapper::enumerate_files(sigDir.c_str(), "*.msig");
	for (unsigned int n = 0; n < sigFileList.size(); ++n) {
		FILE* hFile = fopenRB(sigFileList[n].c_str());
		if (!hFile){
			continue;;
		}
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
			sigMap.insert(std::make_pair(funcSig.c_str(), funcName.c_str()));
		}
		qfclose(hFile);
	}
}

ECSigScanner& ECSigScanner::Instance()
{
	return gECSigScanner;
}

bool ECSigScanner::ScanECSigFunction(ESymbol* symbolTable)
{
	std::multimap<std::string, std::string> map_MSig;
	initMSigMap(map_MSig);

	//没特征库就不用识别特征了
	if (!map_MSig.size()) {
		return true;
	}
	for (unsigned int idx = 0; idx < get_func_qty(); ++idx) {
		func_t* pFunc = getn_func(idx);
		if (!pFunc) {
			continue;
		}
		if (pFunc->start_ea < symbolTable->userCodeStartAddr || pFunc->start_ea >= symbolTable->userCodeEndAddr) {
			continue;
		}
		std::string goodMD5 = GetFunctionMD5(pFunc->start_ea);
	}
	return true;
}

std::string ECSigScanner::GetFunctionMD5(unsigned int funcAddr, bool bFuzzy /*= false*/)
{
	bFuzzyFlag = bFuzzy;
	funcIndexMap.clear();
	funcIndex = 0;
	return calFunctionMD5(funcAddr, true);
}

std::string ECSigScanner::calFunctionMD5(unsigned int funcStartEA, bool bUseCache, bool bSubFunc)
{
	if (bUseCache) {
		std::map<unsigned int, std::string>::iterator it = sigCacheMap.find(funcStartEA);
		if (it != sigCacheMap.end()) {
			return it->second;
		}
	}
	
	//记录函数顺序
	funcIndexMap[funcStartEA] = funcIndex++;

	std::string retMD5;
	if (!isEStandardFunction(funcStartEA)) {
		return retMD5;
	}
	unsigned int funcEndEA = guessFuncEndAddr(funcStartEA);
	if (funcEndEA == -1) {
		return retMD5;
	}
	this->currentEA = funcStartEA;
	this->endEA = funcEndEA;
	
	//开始处理数据
	bool bFinish = false;
	insn_t tmpIns;
	do
	{
		int insLen = decode_insn(&tmpIns, this->currentEA);
		if (insLen) {
			bFinish = genSig_main(tmpIns,bSubFunc);
			this->currentEA = this->currentEA + insLen;
		}
		else {
			bFinish = genSig_rawBinary();
		}
		if (bFinish) {
			break;
		}
	} while (this->currentEA <= this->endEA);
	

	return retMD5;
}

//先看看ida默认的函数尾部的情况

unsigned int checkIDAEndAddr(unsigned int endEA)
{
	insn_t tmpIns;
	unsigned int retAddr = decode_prev_insn(&tmpIns, endEA);
	if (retAddr == -1 || tmpIns.itype != NN_retn) {
		return -1;
	}
	unsigned int popInsAddr = decode_prev_insn(&tmpIns, retAddr);
	if (popInsAddr == -1) {
		return -1;
	}
	//必须是pop ebp
	if (tmpIns.itype != NN_pop || tmpIns.ops[0].reg != 5) {
		return -1;
	}
	unsigned int movInsAddr = decode_prev_insn(&tmpIns,popInsAddr);
	if (movInsAddr == -1) {
		return -1;
	}
	//必须是mov esp,ebp
	if (tmpIns.itype != NN_mov || tmpIns.ops[0].reg != 4 || tmpIns.ops[1].reg != 5) {
		return -1;
	}
	return endEA;
}

unsigned int ECSigScanner::guessFuncEndAddr(unsigned int funcAddr)
{
	func_t* pFunc = get_func(funcAddr);
	if (!pFunc) {
		return -1;
	}
	unsigned int retAddr = checkIDAEndAddr(pFunc->end_ea);
	if (retAddr != -1) {
		return retAddr;
	}
	//找个靠谱的地址
	segment_t* pSegment = getseg(pFunc->start_ea);
	if (!pSegment) {
		return BADADDR;
	}
	std::set<unsigned int> endAddrList;
	compiled_binpat_vec_t binPat;
	parse_binpat_str(&binPat, 0x0, "8B E5 5D", 16);
	unsigned int startAddr = pFunc->start_ea;
	unsigned int maxEndAddr = pFunc->end_ea + 0x10;
	while (true) {
		unsigned int searchAddr = bin_search2(startAddr, pSegment->end_ea, binPat, 0x0);
		if (searchAddr == BADADDR) {
			continue;
		}
		searchAddr = searchAddr + 3;
		insn_t tmpIns;
		if (!decode_insn(&tmpIns, searchAddr)) {
			continue;
		}
		if (searchAddr + tmpIns.size > maxEndAddr) {
			break;
		}
		if (tmpIns.itype == NN_retn) {
			endAddrList.insert(searchAddr + tmpIns.size);
		}
		startAddr = searchAddr;
	}
	if (endAddrList.size() == 0) {
		return -1;
	}
	std::set<unsigned int>::iterator it = endAddrList.cend();
	it--;
	return *it;
}

bool ECSigScanner::genSig_rawBinary()
{
	ea_t nextCodeAddr = find_code(this->currentEA, SEARCH_DOWN | SEARCH_NOSHOW);
	if (nextCodeAddr >= this->endEA) {
		return true;
	}
	//IDAWrapper::get_bytes();
	//unsigned char* pData = SectionManager::LinearAddrToVirtualAddr(startAddr);
	//for (unsigned int n = 0; n < nextCodeAddr - startAddr; ++n) {
	//	tmpSig.append(UCharToStr(pData[n]));
	//}
	//vec_SaveSig.push_back(tmpSig);
	this->currentEA = nextCodeAddr;
	return false;
}

bool ECSigScanner::genSig_main(insn_t& tmpIns, bool bSubFunc)
{
	switch (tmpIns.itype)
	{
	case NN_call:
		break;
	case NN_inc:
	case NN_dec:
	case NN_push:
		return genSig_flexSingleInst(tmpIns);
	case NN_add:
	case NN_sub:
	case NN_cmp:
	case NN_mov:
		return genSig_flexDoubleInst(tmpIns);
	default:
		break;
	}
	return false;
}

bool ECSigScanner::genSig_flexSingleInst(insn_t& ins)
{
	SigElement tmpElement;
	tmpElement.insAddr = ins.ea;
	tmpElement.insType = ins.itype;
	tmpElement.type = SIG_NORMAL;

	genSig_PerfectOp(ins.ops[0], tmpElement);
	return false;
}

bool ECSigScanner::genSig_flexDoubleInst(insn_t& ins)
{
	SigElement tmpElement;
	tmpElement.insAddr = ins.ea;
	tmpElement.insType = ins.itype;
	tmpElement.type = SIG_NORMAL;

	genSig_PerfectOp(ins.ops[0], tmpElement);
	genSig_PerfectOp(ins.ops[1], tmpElement);
	return false;
}

void ECSigScanner::genSig_PerfectOp(op_t& op, SigElement& outSig)
{
	outSig.sig.push_back(op.type);
	switch (op.type)
	{
	case o_void:
		break;
	case o_reg:
		outSig.sig.push_back(op.reg);
		break;
	case o_mem:
		break;
	case o_phrase:
		outSig.sig.push_back(0x1);
		break;
	case o_displ:
		outSig.sig.push_back(op.phrase);
		break;
	default:
		break;
	}
}


void ECSigScanner::genSig_FuzzyOp(op_t& op, SigElement& outSig)
{

}