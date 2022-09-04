#include "MicrocodeFixer.h"
#include "../ESymbol.h"
#include <hexrays.hpp>
#include <ua.hpp>
#include <allins.hpp>

//microcode修复

void microGen_PushDefaultArg(codegen_t& cdg)
{
	//cdg.insn.create_stkvar(cdg.insn.ops[0], 0x30, 0);
	//mop_t l;
	//l.make_number(0, 4);
	//cdg.emit(m_push, &l, 0, 0);

	//reanalyze_function(cdg.mba->get_curfunc());

	//cdg.emit(m_push, &l, 0, 0);
	//cdg.emit(m_push, &l, 0, 0);
	//cdg.emit(m_push, &l, 0, 0);
	//cdg.emit(m_push, &l, 0, 0);
	
	//cdg.mb->mark_lists_dirty();
}

void blk_merge(mblock_t* dst, mblock_t* src)
{
	minsn_t* src_insn = src->head;
	minsn_t* dst_insn = NULL;
	while (src_insn != NULL)
	{
		minsn_t* new_insn = new minsn_t(*src_insn);
		new_insn->ea = src_insn->ea;
		dst_insn = dst->insert_into_block(new_insn, dst->tail);
		src_insn = src_insn->next;
	}
}

void microFix_PushDefaultArg(mblock_t* blk, minsn_t* ins, ESymbol* symbolTable)
{
	insn_t ins_MovArg;
	if (decode_insn(&ins_MovArg, ins->ea - 0x5) == 0x0 || ins_MovArg.itype != NN_mov) {
		return;
	}

	blk_merge(blk, blk->nextb);
	blk->mark_lists_dirty();
	//int argCount = ins_MovArg.ops[1].value;
	//for (int n = 0; n < argCount; n++) {
	//	minsn_t* mPush = new minsn_t(ins->ea - 0x5);
	//	mPush->opcode = m_push;
	//	mPush->l.make_number(4, 0);
	//	blk->nextb->insert_into_block(mPush, ins->prev);
	//}
	//blk->mark_lists_dirty();
	//blk->remove_from_block(ins);
	return;
}

//没有人会在乎你是否释放内存

void microFix_KrnlFreeMem(mblock_t* blk, minsn_t* ins, ESymbol* symbolTable)
{
	ins->_make_nop();
}

struct MicrocodeFixer :public optinsn_t
{
private:
	ESymbol* symbolTable;
public:
	void setSymbolTable(ESymbol* s)
	{
		symbolTable = s;
	}
	int func(mblock_t* blk, minsn_t* ins, int optflags)
	{
		if (ins->opcode == m_call)
		{
			eSymbolFuncType symbolT = symbolTable->GetFuncSymbolType(ins->l.r);
			if (!symbolT) {
				return 0;
			}
			switch (symbolT)
			{
			case eFunc_KrnlFreeMem:
				microFix_KrnlFreeMem(blk, ins, symbolTable);
				break;
			default:
				break;
			}
		}
		return 0;
	}
};
MicrocodeFixer gMicrocodeFixer;


struct BlockFixer :public optblock_t
{
	int idaapi func(mblock_t* blk)
	{
		struct insVistor :public minsn_visitor_t
		{
			int idaapi visit_minsn(void)
			{
				if (curins->opcode != m_call) {
					return 0;
				}


				return 0;
			}
		};
		insVistor vistor;
		blk->for_all_insns(vistor);

		minsn_t* pHeadIns = blk->head;


		return 0;
	}
};

struct MicroCodeGenerator :public microcode_filter_t
{
public:
	ESymbol* symbolTable;
public:
	void setSymbolTable(ESymbol* s)
	{
		symbolTable = s;
	}
private:
	bool match(codegen_t& cdg)
	{
		if (cdg.insn.itype != NN_call) {
			return false;
		}
		//eSymbolFuncType symbolT = symbolTable->GetFuncSymbolType(cdg.insn.ops[0].addr);
		//if (!symbolT) {
		//	return false;
		//}
		//if (symbolT == eFunc_PushDefaultArg) {
		//	return true;
		//}
		return false;
	}
	merror_t apply(codegen_t& cdg)
	{
		microGen_PushDefaultArg(cdg);
		return MERR_OK;
	}
};
MicroCodeGenerator gMicrocodeGenerator;


struct MyBlockTester : public optblock_t
{
	int idaapi func(mblock_t* blk)
	{
		if (blk->type != BLT_1WAY) {
			return 0;
		}
		if (!blk->tail) {
			return 0;
		}
		if (blk->tail->opcode != m_call) {
			return 0;
		}
		if (blk->tail->l.r != 0x00401010) {
			return 0;
		}
		minsn_t* tmpIns = new minsn_t(0x00401021);
		tmpIns->opcode = m_push;
		tmpIns->l.make_number(0, 4, 0x00401021);
		blk->insert_into_block(tmpIns,blk->tail);
		int a = 0;
		return 0;
	}
};
MyBlockTester gBlockTester;

void InstallMicroCodeFixer(ESymbol& symbol)
{
	gMicrocodeFixer.setSymbolTable(&symbol);
	install_optinsn_handler(&gMicrocodeFixer);

	//gMicrocodeGenerator.setSymbolTable(&symbol);
	//install_microcode_filter(&gMicrocodeGenerator, true);


	//install_optblock_handler(&gBlockTester);
}

void UnInstallMicroCodeFixer()
{
	remove_optinsn_handler(&gMicrocodeFixer);
}


