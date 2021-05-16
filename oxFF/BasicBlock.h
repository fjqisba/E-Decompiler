#pragma once
#include <pro.h>
#include <set>

class BasicBlock
{
public:
	BasicBlock(ea_t entry);	
	~BasicBlock();
public:
	//添加指令
	void AppendInstruction(ea_t instAddr);
public:
	ea_t m_entry;  //基本块入口
	ea_t m_end;    //基本块出口
private:
	std::set<ea_t> m_SaveInstructionAddr;
};