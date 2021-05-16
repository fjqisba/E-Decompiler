#include "BasicBlock.h"

BasicBlock::BasicBlock(ea_t entry)
{
	m_entry = entry;
}

BasicBlock::~BasicBlock()
{

}

void BasicBlock::AppendInstruction(ea_t instAddr)
{
	m_SaveInstructionAddr.insert(instAddr);
}