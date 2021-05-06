#pragma once
#include <pro.h>
#include <unordered_set>

struct LibScanResult
{
	qstring LibName;
	qstring Guid;
	struct FuncScanResult
	{
		ea_t FuncAddr;
		qstring FuncName;
	};
	qvector<FuncScanResult> Vec_Funcs;
};

class ESigScanner
{
public:
	//扫描并识别支持库函数
	static bool ScanLibFunction(ea_t lpLibStartAddr, uint32 dwLibCount);
	//扫描并识别易语言基础命令
	static bool ScanBasicFunction();
private:
	//用于记录哪些地址是库函数的集合
	static std::unordered_set<ea_t> mHash_LibFunc;
};
