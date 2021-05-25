#include "EsigScanner.h"
#include "EDecompiler.h"
#include <bytes.hpp>
#include <funcs.hpp>
#include <fpro.h>
#include <name.hpp>
#include <diskio.hpp>
#include "TrieTree.h"
#include "ECSigParser.h"
#include "common/public.h"

struct EsigInfo
{
	qstring Category;
	qstring Name;
	qstring Description;
	qstring Path;
};

qvector<EsigInfo> g_EsigList;

std::unordered_set<ea_t> ESigScanner::mHash_LibFunc;

void setFuncName(ea_t addr, const char* funcName)
{
	qstring oldName = get_name(addr);
	if (oldName.find("sub_") != qstring::npos) {
		qstring newName;
		acp_utf8(&newName, funcName);
		set_name(addr, newName.c_str(), SN_NOWARN);
	}
}

int EnumEsig(const char* lpMapPath, void* ud)
{
	FILE* hFile = qfopen(lpMapPath, "rb");
	if (!hFile) {
		return 0;
	}

	uint32 dwSize = qfsize(hFile);
	char* pMap = (char*)malloc(dwSize);
	qfread(hFile, pMap, dwSize);

	qstring Sig = pMap;
	qstring Config = GetMidString(Sig, "******Config******\r\n", "******Config_End******", 0);
	qstring Name = GetMidString(Sig, "Name=", "\r\n", 0);
	qstring Description = GetMidString(Sig, "Description=", "\r\n", 0);

	if (!Name.size()) {
		return 0;
	}

	EsigInfo saveinfo;
	saveinfo.Name = Name;
	saveinfo.Description = Description;
	saveinfo.Path = lpMapPath;

	g_EsigList.push_back(saveinfo);

	if (pMap) {
		free(pMap);
	}

	qfclose(hFile);
	return 0;
}

bool ESigScanner::ScanBasicFunction()
{
	TrieTree BASICTREE;
	qstring LibPath;
	LibPath.sprnt("%s\\esig\\易语言基础命令.esig", idadir(PLG_SUBDIR));

	std::set<ea_t> m_Hash;
	if (!BASICTREE.LoadSig(LibPath.c_str())) {
		return false;
	}

	for (unsigned int idx = 0; idx < get_func_qty(); ++idx) {
		func_t* pFunc = getn_func(idx);
		if (!pFunc) {
			continue;
		}
		if (mHash_LibFunc.count(pFunc->start_ea)) {
			continue;
		}
		char* pFuncName = BASICTREE.MatchFunc(SectionManager::LinearAddrToVirtualAddr(pFunc->start_ea));
		if (pFuncName) {
			m_Hash.insert(pFunc->start_ea);
			setFuncName(pFunc->start_ea, pFuncName);
		}
	}
	ECSigParser::InitECSigBasciFunc(m_Hash);
	return true;
}

bool ESigScanner::ScanLibFunction(ea_t lpLibStartAddr, uint32 dwLibCount)
{
	mHash_LibFunc.clear();

	for (unsigned int nLibIndex = 0; nLibIndex < dwLibCount; ++nLibIndex) {
		LIB_INFO tmpLibInfo;
		get_bytes(&tmpLibInfo, sizeof(LIB_INFO), get_dword(lpLibStartAddr));
		lpLibStartAddr = lpLibStartAddr + 4;

		//判断是否符合支持库格式
		if (tmpLibInfo.m_dwLibFormatVer != 0x1312D65) {
			continue;
		}

		//支持库缺少函数
		if (!tmpLibInfo.m_nCmdCount || !tmpLibInfo.m_lpCmdsFunc) {
			continue;
		}

		qstring LibPath;
		LibPath.sprnt("%s\\esig\\%s.esig", idadir(PLG_SUBDIR), get_shortstring(tmpLibInfo.m_lpName).c_str());

		TrieTree ESIGTREE;
		if (!ESIGTREE.LoadSig(LibPath.c_str())) {
			continue;
		}

		uint32* pFuncBuf = (uint32*)qalloc(tmpLibInfo.m_nCmdCount * 4);
		get_bytes(pFuncBuf, tmpLibInfo.m_nCmdCount * 4, tmpLibInfo.m_lpCmdsFunc);
		for (unsigned int nFuncIndex = 0; nFuncIndex < tmpLibInfo.m_nCmdCount; ++nFuncIndex) {
			ea_t funcAddr = pFuncBuf[nFuncIndex];

			mHash_LibFunc.insert(funcAddr);
			add_func(funcAddr);
			char* pFuncName = ESIGTREE.MatchFunc(SectionManager::LinearAddrToVirtualAddr(funcAddr));
			if (!pFuncName) {
				qstring errorFuncName;
				errorFuncName.sprnt("未知命令_%a", funcAddr);
				setFuncName(funcAddr, errorFuncName.c_str());
				continue;
			}
			setFuncName(funcAddr, pFuncName);
		}
		qfree(pFuncBuf);
	}
	return true;
}