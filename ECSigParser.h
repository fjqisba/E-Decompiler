#pragma once
#include <pro.h>

class ECSigParser
{
public:
	//生成易语言模块函数特征码
	static int GenerateECSig(ea_t startAddr);

private:
	//检查是否为易语言标准函数，参数为函数起始地址
	static bool IsEStandardFunction(ea_t startAddr);
	//寻找易语言标准函数尾部,参数为函数起始地址,返回最后一条指令的地址
	static ea_t SeachEFuncEnd(ea_t startAddr);
};