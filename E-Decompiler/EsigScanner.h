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
	
	
private:
	//用于记录哪些地址是库函数的集合
	static std::unordered_set<ea_t> mHash_LibFunc;
};
