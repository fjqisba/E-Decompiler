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
	install_hexrays_callback(CTreeFixCallback, &eSymbol);
}

CTreeFixer::~CTreeFixer()
{
	//remove_hexrays_callback(CTreeFixCallback, &eSymbol);
}