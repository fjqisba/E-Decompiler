#include "ShowImports.h"
#include "../ESymbol.h"
#include <pro.h>
#include <kernwin.hpp>
#include <ua.hpp>
#include <allins.hpp>
#include <funcs.hpp>
#include "../Utils/IDAWrapper.h"

int ShowImports(void* ud)
{
	ESymbol& symbolTable = *(ESymbol*)ud;

	struct ImportsApi
	{
		int order;
		unsigned int addr;
		std::string api;
	};

	struct chooser_ImportsInfo :public chooser_multi_t
	{
	private:
		std::vector<ImportsApi> importsList;
		ESymbol& symbol;
	protected:
		const int widths_[4] = { 10,10,20,20 };
		const char* header_[4] = { "order","addr","funcName","api" };
	public:
		chooser_ImportsInfo(const char* title, ESymbol& s) :chooser_multi_t(0, qnumber(widths_), widths_, header_, title), symbol(s) {

			auto xRefList = IDAWrapper::getAllCodeXrefAddr(symbol.krnlJmp.Jmp_MCallDllCmd);
			for (unsigned int n = 0; n < xRefList.size(); ++n) {
				insn_t orderIns;
				if (decode_prev_insn(&orderIns, xRefList[n]) == BADADDR || orderIns.itype != NN_mov) {
					continue;
				}
				unsigned int order = orderIns.ops[1].value;
				if (order >= symbol.tmpImportsApiList.size()) {
					continue;
				}
				ImportsApi tmpApi;
				tmpApi.order = order;
				tmpApi.addr = xRefList[n];
				tmpApi.api = symbol.tmpImportsApiList[order];
				importsList.push_back(tmpApi);
			}
		}
	private:
		~chooser_ImportsInfo() {}
		const void* get_obj_id(size_t* len) const
		{
			*len = strlen(title);
			return title;
		}
		size_t idaapi get_count(void) const
		{
			return importsList.size();
		}
		cbres_t idaapi enter(sizevec_t* sel)
		{
			if (sel->size()) {
				jumpto(importsList[sel->at(0)].addr);
			}
			return cbres_t();
		}
		void idaapi get_row(qstrvec_t* cols_, int* icon_, chooser_item_attrs_t* attrs, size_t n) const
		{
			qstrvec_t& cols = *cols_;
			cols[0].sprnt("%a", importsList[n].order);
			cols[1].sprnt("%08X", importsList[n].addr);
			get_func_name(&cols[2],importsList[n].addr);
			cols[3].sprnt("%s", importsList[n].api.c_str());
		}
	};

	qstring title;
	acp_utf8(&title, "用户导入表");
	chooser_ImportsInfo* pEventWindow = new chooser_ImportsInfo(title.c_str(), symbolTable);
	pEventWindow->choose();
	return 0;
}

