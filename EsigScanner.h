#pragma once
#include <pro.h>

class ESigScanner
{
public:
	//扫描并识别支持库函数
	static bool ScanLibFunction(ea_t lpLibStartAddr, uint32 dwLibCount);
};
