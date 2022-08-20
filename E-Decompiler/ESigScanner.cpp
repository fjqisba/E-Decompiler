#include "EsigScanner.h"
#include "EDecompiler.h"
#include <bytes.hpp>
#include <funcs.hpp>
#include <fpro.h>
#include <name.hpp>
#include <diskio.hpp>
#include <typeinf.hpp>
#include "TrieTree.h"
#include "ECSigParser.h"
#include <kernwin.hpp>
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

//bool ESigScanner::ScanLibFunction(ea_t lpLibStartAddr, uint32 dwLibCount)
//{
//	
//	return true;
//}