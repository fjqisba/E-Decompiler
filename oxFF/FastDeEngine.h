#pragma once
#include <pro.h>
#include <funcs.hpp>


//——————————————用于存储反编译结果的结构体————————————————

enum PushType
{
	PUSH_Reg,
	PUSH_Addr,
};

struct StackContent
{
	PushType type;
	uint32 value;
};


//局部变量
class LocalVariable
{

};

//动态堆栈
class DynamicStack:public qstack<StackContent>
{
public:

};

class FastDeEngine
{
public:
	FastDeEngine();
	bool DoFastDecompile(func_t* pFunc);
private:
	void Handler_push();
	void Handler_sub();
	void Handler_mov();
private:
	enum DecompileState
	{
		ESTATE_Error = -1,
		ESTATE_Start,       //开始分析
		ESTATE_FuncHead,    //函数头,push ebp,mov ebp,esp
	};
private:
	DecompileState m_DecompileState;
	qlist<ea_t> m_analyzeQuene;
	int m_VarStackSize = 0;
	DynamicStack m_DynamicStack;
	LocalVariable m_LocalVarList;
};