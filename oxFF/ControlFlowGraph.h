#pragma once
#include "BasicBlock.h"
#include <unordered_map>
#include <unordered_set>

class ControlFlowGraph
{
public:
	ControlFlowGraph(ea_t entry);
private:
	//存储着全部的基本块,     BasicBlock entry -> BasicBlock
	std::unordered_map<ea_t, BasicBlock> m_nodes;

	//存储着所有基本块的父节点数组  BasicBlock entry -> its Parants
	std::unordered_map<ea_t, std::unordered_set<ea_t>> m_parents;
};