#include "ShowEventList.h"
#include "../ESymbol.h"
#include <pro.h>
#include <kernwin.hpp>
#include <vector>
#include "../EAppControl/EAppControl.h"

int ShowEventList(void* ud)
{
	ESymbol& symbolTable = *(ESymbol*)ud;

	struct eventInfo
	{
		unsigned int windowID;
		unsigned int controlID;
		unsigned int eventAddr;
		std::string controlType;
		std::string eventName;
	};

	struct chooser_EventInfo :public chooser_multi_t
	{
	private:
		std::vector<eventInfo> controlList;
		ESymbol& symbol;
	protected:
		const int widths_[5] = { 10,10,10,10,50 };
		const char* header_[5] = { "windowID","ControlID","Address","ControlType","EventName" };
	public:
		chooser_EventInfo(const char* title, ESymbol& s) :chooser_multi_t(0, qnumber(widths_), widths_, header_, title), symbol(s) {

			std::vector<eventInfo> eventInfoList;
			for (unsigned int nControlIndex = 0; nControlIndex < symbol.allControlList.size(); ++nControlIndex) {
				EAppControl* eControl = symbol.allControlList[nControlIndex];
				if (!eControl) {
					continue;
				}
				for (unsigned int n = 0; n < eControl->eventList.size(); ++n)
				{
					eventInfo tmpEvent;
					tmpEvent.windowID = eControl->windowID;
					tmpEvent.controlID = eControl->controlId;
					tmpEvent.eventAddr = eControl->eventList[n].eventAddr;
					tmpEvent.controlType = eControl->controlTypeName;
					tmpEvent.eventName = "_" + eControl->controlName + "_" + eControl->GetEventName(eControl->eventList[n].eventIndex);
					controlList.push_back(tmpEvent);
				}
			}
		}
	private:
		~chooser_EventInfo() {}
		const void* get_obj_id(size_t* len) const
		{
			*len = strlen(title);
			return title;
		}
		size_t idaapi get_count(void) const
		{
			return controlList.size();
		}
		cbres_t idaapi enter(sizevec_t* sel)
		{
			if (sel->size()) {
				jumpto(controlList[sel->at(0)].eventAddr);
			}
			return cbres_t();
		}
		void idaapi get_row(qstrvec_t* cols_, int* icon_, chooser_item_attrs_t* attrs, size_t n) const
		{
			qstrvec_t& cols = *cols_;
			cols[0].sprnt("0x%08X", controlList[n].windowID);
			cols[1].sprnt("0x%08X", controlList[n].controlID);
			cols[2].sprnt("%08X", controlList[n].eventAddr);
			acp_utf8(&cols[3], controlList[n].controlType.c_str());
			acp_utf8(&cols[4], controlList[n].eventName.c_str());
		}
	};

	qstring title;
	acp_utf8(&title,"控件事件信息");
	chooser_EventInfo* pEventWindow = new chooser_EventInfo(title.c_str(),symbolTable);
	pEventWindow->choose();
	return 0;
}
