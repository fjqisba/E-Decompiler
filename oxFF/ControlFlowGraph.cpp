#include "ControlFlowGraph.h"
#include <ua.hpp>
#include <allins.hpp>

bool IsRetInst(insn_t& ins)
{
	if (ins.itype >= NN_retn && ins.itype <= NN_retf) {
		return true;
	}
	return false;
}

bool IsLoopInst(insn_t& ins)
{
	if (ins.itype >= NN_loopw && ins.itype <= NN_loopqne) {
		return true;
	}
	return false;
	/*if (ins.itype == NN_loopw) {
		return true;
	}
	if (ins.itype == NN_loop) {
		return true;
	}
	if (ins.itype == NN_loopd) {
		return true;
	}
	if (ins.itype == NN_loopq) {
		return true;
	}
	if (ins.itype == NN_loopwe) {
		return true;
	}
	if (ins.itype == NN_loope) {
		return true;
	}
	if (ins.itype == NN_loopde) {
		return true;
	}
	if (ins.itype == NN_loopqe) {
		return true;
	}
	if (ins.itype == NN_loopwne) {
		return true;
	}
	if (ins.itype == NN_loopne) {
		return true;
	}
	if (ins.itype == NN_loopdne) {
		return true;
	}
	if (ins.itype == NN_loopqne) {
		return true;
	}*/
}

bool IsUnConditionJumpInst(insn_t& ins)
{
	if (ins.itype >= NN_jmp && ins.itype <= NN_jmpshort) {
		return true;
	}
	return false;
}

bool IsConditionJumpInst(insn_t& ins)
{
	if (ins.itype >= NN_ja && ins.itype <= NN_jz) {
		return true;
	}
	return false;
	//if (ins.itype == NN_ja) {
	//	return true;
	//}
	//if (ins.itype == NN_jae) {
	//	return true;
	//}
	//if (ins.itype == NN_jb) {
	//	return true;
	//}
	//if (ins.itype == NN_jbe) {
	//	return true;
	//}
	//if (ins.itype == NN_jc) {
	//	return true;
	//}
	//if (ins.itype == NN_jcxz) {
	//	return true;
	//}
	//if (ins.itype == NN_jecxz) {
	//	return true;
	//}
	//if (ins.itype == NN_jrcxz) {
	//	return true;
	//}
	//if (ins.itype == NN_je) {
	//	return true;
	//}
	//if (ins.itype == NN_jg) {
	//	return true;
	//}
	//if (ins.itype == NN_jge) {
	//	return true;
	//}
	//if (ins.itype == NN_jl) {
	//	return true;
	//}
	//if (ins.itype == NN_jle) {
	//	return true;
	//}
	//if (ins.itype == NN_jna) {
	//	return true;
	//}
	//if (ins.itype == NN_jnae) {
	//	return true;
	//}
	//if (ins.itype == NN_jnb) {
	//	return true;
	//}
	//if (ins.itype == NN_jnbe) {
	//	return true;
	//}
	//if (ins.itype == NN_jnc) {
	//	return true;
	//}
	//if (ins.itype == NN_jne) {
	//	return true;
	//}
	//if (ins.itype == NN_jng) {
	//	return true;
	//}
	//if (ins.itype == NN_jnge) {
	//	return true;
	//}
	//if (ins.itype == NN_jnl) {
	//	return true;
	//}
	//if (ins.itype == NN_jnle) {
	//	return true;
	//}
	//if (ins.itype == NN_jno) {
	//	return true;
	//}
	//if (ins.itype == NN_jnp) {
	//	return true;
	//}
	//if (ins.itype == NN_jns) {
	//	return true;
	//}
	//if (ins.itype == NN_jnz) {
	//	return true;
	//}
	//if (ins.itype == NN_jo) {
	//	return true;
	//}
	//if (ins.itype == NN_jp) {
	//	return true;
	//}
	//if (ins.itype == NN_jpe) {
	//	return true;
	//}
	//if (ins.itype == NN_jpo) {
	//	return true;
	//}
	//if (ins.itype == NN_js) {
	//	return true;
	//}
	//if (ins.itype == NN_jz) {
	//	return true;
	//}
	//if (ins.itype == NN_jmp) {
	//	return true;
	//}
	//if (ins.itype == NN_jmpfi) {
	//	return true;
	//}
	//if (ins.itype == NN_jmpni) {
	//	return true;
	//}
	//if (ins.itype == NN_jmpshort) {
	//	return true;
	//}
}

ControlFlowGraph::ControlFlowGraph(ea_t entry)
{
	//qlist<ea_t> analyzeQuene;
	//std::unordered_set<ea_t> visited;

	//ea_t currentEA = entry;
	//analyzeQuene.push_back(currentEA);
	//while (!analyzeQuene.empty()) {
	//	currentEA = analyzeQuene.front();
	//	analyzeQuene.pop_front();

	//	visited.insert(currentEA);

	//	//开始分析一个基本块
	//	BasicBlock eBlock(currentEA);
	//	while (true) {
	//		insn_t tmpIns;
	//		int insLen = decode_insn(&tmpIns, currentEA);
	//		if (!insLen) {
	//			msg("decode %a instruction error\n", currentEA);
	//			return;
	//		}

	//		eBlock.AppendInstruction(currentEA);
	//		if (IsConditionJumpInst(tmpIns)) {
	//			m_nodes[eBlock.m_entry] = eBlock;
	//			analyzeQuene.push_back(tmpIns.ops[0].addr);
	//			analyzeQuene.push_back(tmpIns.ea + insLen);
	//			break;
	//		}
	//		else if (IsUnConditionJumpInst(tmpIns)) {
	//			currentEA = tmpIns.ops[0].addr;
	//			continue;
	//		}
	//		else if (IsLoopInst(tmpIns)) {
	//			msg("To do...ControlFlowGraph[Loop]\n");
	//		}
	//		else if (IsRetInst(tmpIns)) {
	//			break;
	//		}
	//		currentEA = currentEA + insLen;
	//	}

	//}
}