#include "CTreeFixer.h"
#include <hexrays.hpp>
#include "../EDecompiler.h"
#include <typeinf.hpp>
#include "../EAppControl/EAppControl.h"
#include "../Utils/Strings.h"
#include <typeinf.hpp>

//嗯，现阶段目的是让代码可读化，不是直接到源码，因此没必要在代码转换细节进行过度优化


void fix_KrnlDllCmd(cexpr_t* e, ESymbol& symbolTable)
{
	carglist_t& argList = *(e->a);
	if (argList.size() < 1) {
		return;
	}
	unsigned int order = argList[0].n->_value;
	if (order >= symbolTable.tmpImportsApiList.size()) {
		return;
	}
	cexpr_t* tmpHelper = create_helper(true, e->a->functype, "%s", symbolTable.tmpImportsApiList[order].c_str());
	e->x->cleanup();
	e->x->replace_by(tmpHelper);
	int a = 0;
}

void fix_KrnlWriteProperty(cexpr_t* e, ESymbol& symbolTable)
{
	carglist_t& argList = *(e->a);
	if (argList.size() != 6) {
		return;
	}

	unsigned int windowId = argList[0].n->_value;
	unsigned int controlID = argList[1].n->_value;
	unsigned int properIndex = argList[2].n->_value;

	EAppControl* appControl = symbolTable.GetEAppControl(windowId, controlID);
	if (!appControl) {
		return;
	}
	//生成函数名字
	qstring helperFuncName;
	helperFuncName.sprnt("%s::%s_写属性_%s", appControl->controlTypeName.c_str(),
		appControl->controlName.c_str(), appControl->GetPropertyName(properIndex).c_str());
	qstring utf8FuncName;
	acp_utf8(&utf8FuncName, helperFuncName.c_str());

	cexpr_t* tmpHelper = create_helper(true, e->a->functype, "%s", utf8FuncName.c_str());
	e->x->cleanup();
	e->x->replace_by(tmpHelper);

	//argList.erase(&argList[0], &argList[4]);
	//argList.pop_back();
}

void fix_KrnlLibFunc(cexpr_t* e, ESymbol& symbolTable)
{
	//参数至少是两个
	carglist_t& argList = *(e->a);
	if (argList.size() < 2) {
		return;
	}

	//unsigned int callLibAddr = argList[0].x->obj_ea;
	//qstring funcName;
	//get_func_name(&funcName,callLibAddr);
	//
	//cexpr_t* tmpHelper = create_helper(true, e->a->functype, "%s", funcName.c_str());
	//e->x->cleanup();
	//e->x->replace_by(tmpHelper);
	//
	//argList.del(argList[1]);
	//argList.del(argList[0]);
}

void fix_KrnlReadProperty(cexpr_t* e, ESymbol& symbolTable)
{
	carglist_t& argList = *(e->a);
	if (argList.size() != 4) {
		return;
	}
	
	unsigned int windowId = argList[0].n->_value;
	unsigned int controlID = argList[1].n->_value;
	unsigned int properIndex = argList[2].n->_value;

	EAppControl *appControl = symbolTable.GetEAppControl(windowId, controlID);
	if (!appControl) {
		return;
	}

	//生成函数名字
	qstring helperFuncName;
	helperFuncName.sprnt("%s::%s_读属性_%s", appControl->controlTypeName.c_str(),
		appControl->controlName.c_str(), appControl->GetPropertyName(properIndex).c_str());
	qstring utf8FuncName;
	acp_utf8(&utf8FuncName,helperFuncName.c_str());

	cexpr_t* tmpHelper = create_helper(true, e->a->functype, "%s", utf8FuncName.c_str());
	e->x->cleanup();
	e->x->replace_by(tmpHelper);

	//clear是清理不干净指针的
	//argList.erase(argList.begin(),argList.end());
}

struct CTreeFixer_Vistor : public ctree_visitor_t
{
	CTreeFixer_Vistor(ESymbol& s) :ctree_visitor_t(CV_FAST),symbolTable(s) {};
	//返回0表示继续遍历
	int idaapi visit_expr(cexpr_t* e) override
	{
		if (e->op == cot_call) {
			eSymbolFuncType symbolT = symbolTable.GetFuncSymbolType(e->x->obj_ea);
			if (!symbolT) {
				return 0;
			}
			switch (symbolT)
			{
			case eFunc_Unknown:
				break;
			case eFunc_KrnlLibFunc:
				fix_KrnlLibFunc(e,symbolTable);
				break;
			case eFunc_KrnlReadProerty:
				fix_KrnlReadProperty(e, symbolTable);
				break;
			case eFunc_KrnlWriteProperty:
				fix_KrnlWriteProperty(e, symbolTable);
				break;
			case eFunc_KrnlDllCmd:
				fix_KrnlDllCmd(e,symbolTable);
				break;
			case eFunc_Strcat:
				break;
			default:
				break;
			}
		}
		return 0;
	}
private:
	ESymbol& symbolTable;
};

void tryFixCTree(cfunc_t* cfunc,ESymbol& symbolTable)
{
	CTreeFixer_Vistor fixer(symbolTable);
	fixer.apply_to(&cfunc->body,NULL);
	
	return;
}

//将块fromBlk移动到块toBlk尾部
void mergeBlocks(mblock_t* dst, mblock_t* src)
{
	auto firstIns = src->head;
	if (!firstIns) {
		return;
	}
	while (true) {
		minsn_t* newIns = new minsn_t(*firstIns);
		newIns->ea = firstIns->ea;
		dst->insert_into_block(newIns, dst->tail);
		if (!firstIns->next) {
			dst->end = firstIns->ea;
			break;
		}
		firstIns = firstIns->next;
	}
}

void copyBlock(mblock_t* dst,mblock_t* src)
{
	minsn_t* nextIns = src->tail;
	while (nextIns != NULL) {
		dst->insert_into_block(new minsn_t(*nextIns),0);
		nextIns = nextIns->prev;
	}
}

// Insert new empty block with attributes copied from src
mblock_t* copyBlockEmpty(mblock_t* src, int insertBefore) {

	mblock_t* dst = src->mba->insert_block(insertBefore);

	// Copy struct members
	dst->flags = src->flags;
	dst->start = src->start;
	dst->end = src->end;
	dst->type = src->type;

	// Copy lists
	dst->dead_at_start = src->dead_at_start;
	dst->mustbuse = src->mustbuse;
	dst->maybuse = src->maybuse;
	dst->mustbdef = src->mustbdef;
	dst->maybdef = src->maybdef;
	dst->dnu = src->dnu;

	// Copy sval_t
	//dst->maxbsp = src->maxbsp;
	//dst->minbstkref = src->minbstkref;
	//dst->minbargref = src->minbargref;

	dst->mark_lists_dirty();
	return dst;

}



void debug_testInert(mblock_t* nextBlk)
{
	auto addr = nextBlk->head->ea - 0x1;
	minsn_t* tmpIns = new minsn_t(addr);
	tmpIns->opcode = m_stx;
	tmpIns->l.make_reg(0x30, 4);
	tmpIns->r.make_reg(0x54, 2);
	tmpIns->d.make_reg(0x50, 4);
	nextBlk->insert_into_block(tmpIns, 0);

	tmpIns = new minsn_t(addr);
	tmpIns->opcode = m_mov;
	tmpIns->l.make_reg(0x64, 2);
	tmpIns->d.make_reg(0x54, 2);
	nextBlk->insert_into_block(tmpIns, 0);

	tmpIns = new minsn_t(addr);
	tmpIns->opcode = m_mov;
	tmpIns->l.make_reg(0x18, 4);
	tmpIns->d.make_reg(0x50, 4);
	nextBlk->insert_into_block(tmpIns, 0);

	tmpIns = new minsn_t(addr);
	tmpIns->opcode = m_mov;
	tmpIns->l.make_reg(0x38, 4);
	tmpIns->d.make_reg(0x30, 4);
	nextBlk->insert_into_block(tmpIns, 0);

	tmpIns = new minsn_t(addr);
	tmpIns->opcode = m_mov;
	tmpIns->l.make_number(0x0, 4);
	tmpIns->d.make_reg(0x38, 4);
	nextBlk->insert_into_block(tmpIns, 0);
}

void debug_checkIns(mblock_t* blk)
{
	minsn_t* nextIns = blk->head;
	while (nextIns != NULL) {
		if (nextIns->ea == 0x0401035) {
			blk->remove_from_block(nextIns);
		}
		nextIns = nextIns->next;
	}
	debug_testInert(blk);
}

void debug_updateIns(mblock_t* blk,minsn_t* updateIns)
{
	for (unsigned int n = 0; n < 15; ++n) {
		minsn_t* tmpIns = new minsn_t(updateIns->ea + 0x1);
		tmpIns->opcode = m_push;
		tmpIns->l.make_number(0, 4);
		blk->insert_into_block(tmpIns, updateIns);
	}
}

void checkBlock(mba_t* mba, mblock_t* blk)
{
	auto nextIns = blk->head;
	while (nextIns != nullptr) {
		if (nextIns->ea == 0x00401016) {
			const char* aaa = nextIns->dstr();
			debug_updateIns(blk,nextIns);
			break;
		}
		nextIns = nextIns->next;
	}

	//再遍历一次
	nextIns = blk->head;
	while (nextIns != nullptr) {
		if (nextIns->ea == 0x00401016) {
			minsn_t* insToRemove = nextIns;
			nextIns = nextIns->next;
			blk->remove_from_block(insToRemove);
			continue;
		}
		nextIns = nextIns->next;
	}


	return;
	if (!blk->tail) {
		return;
	}
	if (blk->tail->opcode != m_call) {
		return;
	}
	



	mblock_t* nextBlk = blk->nextb;
	//nextBlk->head->l.make_number(0x12345678, 4);

	for (unsigned int n = 0; n < 6; ++n) {
		debug_testInert(nextBlk);
		continue;
		minsn_t* tmpIns = new minsn_t(nextBlk->head->ea);
		tmpIns->opcode = m_sub;
		tmpIns->l.make_reg(0x18, 4);
		tmpIns->r.make_number(4, 4);
		tmpIns->d.make_reg(0x18, 4);
		nextBlk->insert_into_block(tmpIns, 0);
	}

	mba->copy_block(blk,nextBlk->serial);
	//minsn_t* coreCallIns = blk->tail->prev;
	//minsn_t* lastIns = blk->tail->prev;
	//while (lastIns != NULL) {
	//	minsn_t* copyIns = new minsn_t(*lastIns);
	//	nextBlk->insert_into_block(copyIns, 0);
	//	lastIns = lastIns->prev;
	//}
	mba->remove_block(blk);
	//nextBlk->build_lists(true);
	//nextBlk->maxbsp = nextBlk->maxbsp + 24;
	
	//nextBlk->optimize_block();
	//copyBlockEmpty(nextBlk,nextBlk->serial);
	//mba->remove_block(nextBlk);
	//对于新出来的block
	/*mblock_t* newBlock = mba->insert_block(nextBlk->serial);
	copyBlock(newBlock, nextBlk);*/
	
	//newBlock->mark_lists_dirty();
	return;
}

void tryFixMicroCode(mba_t* mba, ESymbol& symbolTable)
{
	mblock_t* startBlock =  mba->get_mblock(0);
	while (startBlock != NULL) {
		checkBlock(mba,startBlock);
		startBlock = startBlock->nextb;
	}

	mba->tmpstk_size = 0x30;
	//mba->fullsize = 0x140;
	//mba->stacksize = 0x3c;
	//mba->inargoff = 0x40;
	//mba->minargref = 0x40;
	//mba->spd_adjust = 0x0;
	//for (unsigned int n = 0; n < finder.pushArgList.size(); ++n) {
	//	minsn_t* callIns = finder.pushArgList[n];
	//	mblock_t* blk = finder.pushBlockList[n];

	//	mergeBlocks(blk->nextb, blk);
	//	mba->remove_block(blk->nextb);
	//	for (unsigned int n = 0; n < 6; ++n) {
	//		minsn_t* pushArg = new minsn_t(callIns->ea - n);
	//		pushArg->opcode = m_push;
	//		pushArg->l.make_number(0, 4);
	//		blk->nextb->insert_into_block(pushArg, callIns);
	//	}
	//	blk->remove_from_block(callIns);
	//	mba->remove_block(finder.pushBlockList[n]);
	//}
}

ssize_t idaapi CTreeFixCallback(void* ud, hexrays_event_t e, va_list va)
{
	ESymbol* symbol = (ESymbol*)ud;
	switch (e)
	{
	case hxe_maturity:
	{
		cfunc_t* cfunc = va_arg(va, cfunc_t*);
		ctree_maturity_t maturity = va_arg(va, ctree_maturity_t);
		if (maturity == CMAT_FINAL) {
			tryFixCTree(cfunc, *symbol);
			return 0;
		}
	}
	break;
	case hxe_microcode:
	{
		mba_t* mba = va_arg(va, mba_t*);
		//tryFixMicroCode(mba,*symbol);
		return 0;
	}
	break;
	case hxe_func_printed:
	{
		cfunc_t* cfunc = va_arg(va, cfunc_t*);
		//strvec_t& svList = cfunc->sv;
		//for (unsigned int n = 0; n < svList.size(); ++n) {
		//	if (svList[n].line.find("ReadProperty") != -1) {
		//		
		//	}
		//}
		//int a = 0;
	}
	break;
	}

	return 0;
}


CTreeFixer::CTreeFixer(ESymbol& symbol):eSymbol(symbol)
{
	
}

CTreeFixer::~CTreeFixer()
{
	mba_t* mba;
	
}


void CTreeFixer::Install()
{
	install_hexrays_callback(CTreeFixCallback, &eSymbol);
}

void CTreeFixer::UnInstall()
{
	remove_hexrays_callback(CTreeFixCallback, &eSymbol);
}

