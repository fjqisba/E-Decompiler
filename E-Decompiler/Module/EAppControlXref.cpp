#include "EAppControlXref.h"
#include "../Utils/Strings.h"
#include <hexrays.hpp>
#include <ua.hpp>
#include <allins.hpp>
#include "../ESymbol.h"
#include "../Utils/IDAWrapper.h"
#include "../EAppControl/EAppControl.h"

#define ACTION_EAPP_CONTROL_XREF "EDecompiler::EAppControlXref"

EAppControlXref::EAppControlXref(ESymbol& s):symbolTable(s)
{

}

void EAppControlXref::RegisterAction(void* owner)
{
	//注册窗口菜单
	std::string menuName = LocalCpToUtf8("控件交叉引用");
	const action_desc_t tmpXrefDesc = {
	sizeof(action_desc_t),ACTION_EAPP_CONTROL_XREF,menuName.c_str(),this,
	owner,nullptr,nullptr,0,ADF_OT_PLUGMOD };
	register_action(tmpXrefDesc);
}

void EAppControlXref::AttachToPopupMenu(TWidget* view, TPopupMenu* p)
{
	attach_action_to_popup(view, p, ACTION_EAPP_CONTROL_XREF, nullptr, SETMENU_INS);
}

unsigned int getCallAddr(unsigned int insAddr)
{
	insn_t callIns;
	if (decode_insn(&callIns, insAddr) == 0) {
		return -1;
	}
	if (callIns.itype != NN_call) {
		return -1;
	}
	return callIns.ops[0].addr;
}

bool EAppControlXref::initXrefData(unsigned int callAddr)
{
	insn_t windowIns;
	insn_t controlIns;

	unsigned int winAddr = -1;
	unsigned int controlAddr = -1;
	unsigned int proertyIndex = 0x0;
	unsigned int libFunc = 0x0;

	if (callAddr == symbolTable.krnlJmp.Jmp_MWriteProperty || callAddr == symbolTable.krnlJmp.Jmp_MReadProperty) {
		winAddr = decode_prev_insn(&windowIns, this->currentAddr);
		if (winAddr == BADADDR || windowIns.itype != NN_push) {
			return false;
		}
		controlAddr = decode_prev_insn(&controlIns, winAddr);
		if (controlAddr == BADADDR || controlIns.itype != NN_push) {
			return false;
		}
		this->windowID = windowIns.ops[0].value;
		this->controlID = controlIns.ops[0].value;
	}
	else {
		winAddr = decode_insn(&windowIns, this->currentAddr - 0xF);
		if (winAddr == BADADDR || windowIns.itype != NN_push) {
			return false;
		}
		controlAddr = decode_insn(&controlIns, this->currentAddr - 0x14);
		if (controlAddr == BADADDR || controlIns.itype != NN_push) {
			return false;
		}
		this->windowID = windowIns.ops[0].value;
		this->controlID = controlIns.ops[0].value;
	}

	this->currentAppControl = symbolTable.GetEAppControl(this->windowID,this->controlID);
	if (!this->currentAppControl) {
		return false;
	}

	return true;
}

int EAppControlXref::activate(action_activation_ctx_t* ctx)
{
	allXrefData.clear();

	this->currentAddr = get_screen_ea();
	unsigned int curCallTarget = getCallAddr(this->currentAddr);
	if (curCallTarget == -1) {
		msg("[E-Decompiler]:invalid call target, addr %08X", this->currentAddr);
		return 0;
	}

	if (initXrefData(curCallTarget) == false) {
		msg("[E-Decompiler]:init xref error, addr %08X", this->currentAddr);
		return 0;
	}

	//遍历得到所有的引用
	std::vector<unsigned int> cRefList = IDAWrapper::getAllCodeXrefAddr(symbolTable.krnlJmp.Jmp_MWriteProperty);
	eAppControlXrefData tmpXrefData;
	for (unsigned int n = 0; n < cRefList.size(); ++n) {
		if (getRWXrefAddrText(cRefList[n],tmpXrefData.text)) {
			tmpXrefData.type = XrefWriteProperty;
			tmpXrefData.xRefAddr = cRefList[n];
			tmpXrefData.text = "写属性_" + tmpXrefData.text;
			allXrefData.push_back(tmpXrefData);
		}
	}

	cRefList = IDAWrapper::getAllCodeXrefAddr(symbolTable.krnlJmp.Jmp_MReadProperty);
	for (unsigned int n = 0; n < cRefList.size(); ++n) {
		if (getRWXrefAddrText(cRefList[n], tmpXrefData.text)) {
			tmpXrefData.type = XrefReadProperty;
			tmpXrefData.xRefAddr = cRefList[n];
			tmpXrefData.text = "读属性_" + tmpXrefData.text;
			allXrefData.push_back(tmpXrefData);
		}
	}

	cRefList = IDAWrapper::getAllCodeXrefAddr(symbolTable.krnlJmp.Jmp_MCallKrnlLibCmd);
	for (unsigned int n = 0; n < cRefList.size(); ++n) {
		if (getExecuteXrefAddrText(cRefList[n], tmpXrefData.text)) {
			tmpXrefData.type = XrefExecute;
			tmpXrefData.xRefAddr = cRefList[n];
			allXrefData.push_back(tmpXrefData);
		}
	}

	if (allXrefData.size() == 0) {
		return 0;
	}

	showXrefList();
	return 0;
}

action_state_t idaapi EAppControlXref::update(action_update_ctx_t* ctx)
{
	return AST_ENABLE_ALWAYS;
}

bool EAppControlXref::getRWXrefAddrText(unsigned int XrefAddr,std::string& outText)
{
	insn_t tmp_PushWindowID;
	insn_t tmp_PushControlID;
	unsigned int pushWindowAddr = decode_prev_insn(&tmp_PushWindowID, XrefAddr);
	if (pushWindowAddr == BADADDR || tmp_PushWindowID.itype != NN_push) {
		return false;
	}
	unsigned int pushControlAddr = decode_prev_insn(&tmp_PushControlID, pushWindowAddr);
	if (pushWindowAddr == BADADDR || tmp_PushControlID.itype != NN_push) {
		return false;
	}
	if (tmp_PushWindowID.ops[0].value != this->windowID || tmp_PushControlID.ops[0].value != this->controlID) {
		return false;
	}
	unsigned int pushProertyIndexAddr = decode_prev_insn(&tmp_PushControlID,pushControlAddr);
	if (pushProertyIndexAddr == BADADDR || tmp_PushControlID.itype != NN_push) {
		return false;
	}
	outText = this->currentAppControl->GetPropertyName(tmp_PushControlID.ops[0].value);
	return true;
}

bool EAppControlXref::getExecuteXrefAddrText(unsigned int XrefAddr, std::string& outText)
{
	insn_t windowIns, controlIns;
	unsigned int winAddr = decode_insn(&windowIns, XrefAddr - 0xF);
	if (winAddr == BADADDR || windowIns.itype != NN_push) {
		return false;
	}
	unsigned int controlAddr = decode_insn(&controlIns, XrefAddr - 0x14);
	if (controlAddr == BADADDR || controlIns.itype != NN_push) {
		return false;
	}
	if (windowIns.ops[0].value != this->windowID || controlIns.ops[0].value != this->controlID) {
		return false;
	}
	if (decode_prev_insn(&controlIns, XrefAddr) == -1 || controlIns.itype != NN_mov) {
		return false;
	}
	qstring funcName;
	get_ea_name(&funcName, controlIns.ops[1].value);
	outText.assign(funcName.c_str(),funcName.length());
	//尝试去除类名
	int iIndex = outText.find('.');
	if (iIndex != -1) {
		outText = outText.substr(iIndex + 1);
	}
	return true;
}

void EAppControlXref::showXrefList()
{
	struct chooser_ControlXrefList :public chooser_t
	{
	protected:
		const int widths_[4] = { 8,7,12,12 };
		const char* header_[4] = { "Direction","Type","Address","Text" };
		std::vector<eAppControlXrefData>& xRefList;
		unsigned int currentAddr;
	public:
		chooser_ControlXrefList(const char* title, unsigned int curEA, std::vector<eAppControlXrefData>& list) :xRefList(list), chooser_t(CH_MODAL, qnumber(widths_), widths_, header_, title)
		{
			currentAddr = curEA;
		}
	private:
		~chooser_ControlXrefList()
		{

		}
		cbret_t idaapi enter(size_t n)
		{
			jumpto(xRefList[n].xRefAddr);
			return cbres_t();
		}
		const void* get_obj_id(size_t* len) const
		{
			*len = strlen(title);
			return title;
		}
		void idaapi get_row(qstrvec_t* cols_, int* icon_, chooser_item_attrs_t* attrs, size_t n) const
		{
			qstrvec_t& cols = *cols_;

			if (xRefList[n].xRefAddr != currentAddr) {
				if (xRefList[n].xRefAddr > currentAddr) {
					cols[0] = "Down";
				}
				else {
					cols[0] = "Up";
				}
			}

			if (xRefList[n].type == XrefWriteProperty) {
				cols[1] = "Write";
			}
			else if (xRefList[n].type == XrefReadProperty) {
				cols[1] = "Read";
			}
			else {
				cols[1] = "Execute";
			}

			cols[2].sprnt("%08X", xRefList[n].xRefAddr);
			if (xRefList[n].type == XrefExecute) {
				cols[3] = qstring(xRefList[n].text.c_str());
			}
			else {
				acp_utf8(&cols[3], xRefList[n].text.c_str());
			}
		}
		size_t idaapi get_count(void) const
		{
			return xRefList.size();
		}
	};

	std::string xRefListTitle = "xref to " + this->currentAppControl->controlName;
	qstring utf8Title;
	acp_utf8(&utf8Title,xRefListTitle.c_str());
	std::sort(allXrefData.begin(), allXrefData.end(), [](eAppControlXrefData& v1, eAppControlXrefData& v2) {
		return v1.xRefAddr < v2.xRefAddr;
		});
	chooser_ControlXrefList* pXRefShowList = new chooser_ControlXrefList(utf8Title.c_str(),
		this->currentAddr, this->allXrefData);
	pXRefShowList->choose();
}
