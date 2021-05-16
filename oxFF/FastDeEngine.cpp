#include "FastDeEngine.h"
#include <ua.hpp>
#include <allins.hpp>

#define REG_ESP 4
#define REG_EBP 5


insn_t CURRENT_Instruction;

FastDeEngine::FastDeEngine()
{
	m_DecompileState = ESTATE_Start;
}

bool Check_MOV_EBP_ESP(insn_t* tmpIns)
{
	if (tmpIns->itype != NN_mov) {
		return false;
	}
	if (tmpIns->ops[0].reg != REG_EBP || tmpIns->ops[1].reg != REG_ESP) {
		return false;
	}
	return true;
}

void FastDeEngine::Handler_mov()
{
	if (m_DecompileState == ESTATE_FuncHead) {

	}
}

void FastDeEngine::Handler_sub()
{
	//函数头中的sub表示申请变量空间大小
	if (m_DecompileState == ESTATE_FuncHead) {
		if (CURRENT_Instruction.ops[0].reg != REG_ESP) {
			m_DecompileState = ESTATE_Error;
			return;
		}
		m_VarStackSize = CURRENT_Instruction.ops[1].value;
		m_analyzeQuene.push_back(CURRENT_Instruction.ip + CURRENT_Instruction.size);
	}
}

void FastDeEngine::Handler_push()
{
	if (m_DecompileState == ESTATE_Start) {
		if (CURRENT_Instruction.ops[0].reg != REG_EBP) {
			m_DecompileState = ESTATE_Error;
			return;
		}
		insn_t SecodeInstruction;
		decode_insn(&SecodeInstruction, CURRENT_Instruction.ip + CURRENT_Instruction.size);
		if (!Check_MOV_EBP_ESP(&SecodeInstruction)) {
			m_DecompileState = ESTATE_Error;
			return;
		}
		m_DecompileState = ESTATE_FuncHead;
		m_analyzeQuene.push_back(SecodeInstruction.ip + SecodeInstruction.size);
		return;
	}

	StackContent content;
	if (CURRENT_Instruction.ops[0].type == o_reg) {
		content.type = PUSH_Reg;
		content.value = CURRENT_Instruction.ops[0].reg;
		m_DynamicStack.push(content);
	}
	else if (CURRENT_Instruction.ops[0].type == o_imm) {
		content.type = PUSH_Addr;
		content.value = CURRENT_Instruction.ops[0].value;
		m_DynamicStack.push(content);
	}
	else {
		msg("[Handler_push]To do...\n");
		return;
	}
	m_analyzeQuene.push_back(CURRENT_Instruction.ip + CURRENT_Instruction.size);
	return;
}

bool FastDeEngine::DoFastDecompile(func_t* pFunc)
{
	m_analyzeQuene.clear();
	m_analyzeQuene.push_back(pFunc->start_ea);
	
	while (!m_analyzeQuene.empty()) {
		ea_t CurrentEA = m_analyzeQuene.front();
		m_analyzeQuene.pop_front();

		decode_insn(&CURRENT_Instruction, CurrentEA);
		switch (CURRENT_Instruction.itype)
		{
		case NN_push:
			Handler_push();
			break;
		case NN_sub:
			Handler_sub();
			break;
		case NN_mov:

			break;
		default:
			break;
		}
	}



	return true;
}