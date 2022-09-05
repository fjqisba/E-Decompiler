#pragma once
#include <string>
#include <map>
#include <vector>
#include <list>

class ESymbol;

enum SIG_TYPE
{
	SIG_NORMAL = 0x0,
	SIG_CALL = 0x1,
	SIG_DATA = 0x2,
};

struct SigElement
{
	SIG_TYPE type;
	unsigned int insAddr;
	unsigned short insType;
	std::vector<unsigned char> sig;
};

class op_t;
class insn_t;
class ECSigScanner
{
public:
	static ECSigScanner& Instance();
public:
	bool ScanECSigFunction(ESymbol* s);
	//获取函数的MD5,bFuzzy表示是否为模糊特征
	std::string GetFunctionMD5(unsigned int funcAddr, bool bFuzzy = false);
private:
	//bUseCache表示是否使用缓存,bSubFunc表示是否为子函数
	std::string calFunctionMD5(unsigned int funcAddr, bool bUseCache = false, bool bSubFunc = false);
	//猜一猜函数的结束地址
	unsigned int guessFuncEndAddr(unsigned int funcAddr);
	//生成原始数据特征,返回true表示结束解析
	bool genSig_rawBinary();
	bool genSig_main(insn_t& tmpIns, bool bSubFunc);

	//处理灵活的单操作数指令,返回true表示结束解析
	bool genSig_flexSingleInst(insn_t& ins);
	//处理灵活的双操作数指令,返回true表示结束解析
	bool genSig_flexDoubleInst(insn_t& ins);

	//生成完美特征
	void genSig_PerfectOp(op_t& op, SigElement& outSig);
	//生成模糊特征
	void genSig_FuzzyOp(op_t& op,SigElement& outSig);

private:
	//key是函数地址,value是函数MD5特征
	std::map<unsigned int, std::string> sigCacheMap;

	//key是函数地址,value是函数索引
	std::map<unsigned int, unsigned int> funcIndexMap;
	//当前函数索引大小
	unsigned int funcIndex;

	//当前生成结果
	std::vector<SigElement> sigResult;


	unsigned int currentEA;
	unsigned int endEA;
	bool bFuzzyFlag;
};