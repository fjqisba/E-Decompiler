#pragma once
#include <pro.h>
#include <set>
#include <map>

class insn_t;
struct eSymbol_KrnlJmp;
class func_t;
class ECSigParser
{
public:
	//生成易语言模块函数特征码
	static int GenerateECSig(ea_t startAddr);
	static void InitECSigKrnl(eSymbol_KrnlJmp& inFunc);
	static void InitECSigBasciFunc(std::map<ea_t, qstring>& mhash);
	static void InitECSigResource(uint32 startAddr, uint32 endAddr);
	static void ScanMSig(const char* sigPath, ea_t rangeStart, ea_t rangeEnd);
	//调试模式下专用，批量生成特征码
	static void Debug_outputECSig();
private:
	//计算一个用户函数的MD5
	static qstring GetFunctionMD5(ea_t FuncStartAddr);
	//————————————————————————————
	//灵活的单操作数指令
	static qstring GetSig_FlexSingleInst(insn_t& ins);
	//灵活的双操作数指令
	static qstring GetSig_FlexDoubleInst(insn_t& ins);
	//浮点指令
	static qstring GetSig_FloatInstA(insn_t& ins);
	static qstring GetSig_FloatInstB(insn_t& ins);
	//逻辑运算指令
	static qstring GetSig_LogicInst(insn_t& ins);
	static qstring GetSig_Imul(insn_t& ins);
	static qstring GetSig_Pop(insn_t& ins);
	static qstring GetSig_LongJmp(insn_t& ins);
	static qstring GetSig_Call(insn_t& ins, qvector<qstring>& vec_saveSig, bool& out_bSkipState);
	static qstring GetSig_Nop(insn_t& ins);
	//检查是否为易语言标准函数，参数为函数起始地址
	static bool IsEStandardFunction(ea_t startAddr);
	//寻找易语言标准函数尾部,参数为函数起始地址,返回最后一条指令的地址
	static ea_t SeachEFuncEnd(func_t* startAddr);
	//判断一个常量是否在用户资源地址范围之内
	static bool IsUserResourceImm(uint32 imm);
	//判断一个偏移地址是否在用户资源地址范围之内
	static bool IsUserResourceOffset(uint32 offset);
	//三段式指令取模糊特征
	static qstring GetInsPattern_Three(insn_t& ins);
	//两段式指令取模糊特征
	static qstring GetInsPattern_Two(insn_t& ins, char offset);
private:
	static eSymbol_KrnlJmp m_KrnlJmp;
	static std::map<ea_t, qstring> mMap_BasicFunc;
	static uint32 m_UserResourceStartAddr;
	static uint32 m_UserResourceEndAddr;
	static std::map<ea_t, qstring> mSave_SubFunc;

	//模糊特征
	static bool bFuzzySig;
};