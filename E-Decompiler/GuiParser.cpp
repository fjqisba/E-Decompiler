#include "GuiParser.h"
#include <bytes.hpp>
#include <kernwin.hpp>
#include <name.hpp>
#include "EDecompiler.h"
#include "ControlInfoWidget.h"
#include "./Utils/Common.h"
#include <QMap>

ea_t m_UserCodeStartAddr = 0;
unsigned int m_EventSum = 0;
ControlInfoWidget* GuiParser::g_ControlInfoWindow = nullptr;

unsigned int GuiParser::GetEventCount()
{
	return m_EventSum;
}

void GuiParser::InitUserCodeStartAddr(ea_t addr)
{
	m_UserCodeStartAddr = addr;
}

//void GuiParser::ClearControlTypeMap()
//{
//	mMap_ControlTypeIndex.clear();
//}
//
//void GuiParser::AddControlType(unsigned int controlTypeId, ControlType_t type)
//{
//	mMap_ControlTypeIndex[controlTypeId] = type;
//}

//bool GuiParser::GetControlInfo(unsigned int controlId, mid_ControlInfo& out_ControlInfo)
//{
//	QMap<unsigned int, ControlIndex>::iterator it = mMap_ControlIndex.find(controlId);
//	if (it != mMap_ControlIndex.end()) {
//		out_ControlInfo = mVec_GuiInfo[it->nWindowIndex].mVec_ControlInfo[it->nControlIndex];
//		return true;
//	}
//	return false;
//}

//ControlType_t GuiParser::GetControlType(unsigned int controlTypeId)
//{
//	QMap<unsigned int, ControlType_t>::iterator it = mMap_ControlTypeIndex.find(controlTypeId);
//	if (it != mMap_ControlTypeIndex.end()) {
//		return *it;
//	}
//	return UnknownControl;
//}

//int GuiParser::MenuHandle_ShowEventInfo()
//{
//	struct eventInfo
//	{
//		ea_t EventAddr;
//		std::string ControlName;
//		std::string EventName;
//	};

//	struct chooser_EventInfo :public chooser_multi_t
//	{
//	public:
//		qvector<eventInfo> vec_EventInfo;
//	protected:
//		const int widths_[3] = { 10,10,50 };
//		const char* header_[3] = { "Address","ControlType","EventName" };
//	public:
//		chooser_EventInfo(const char* title) :chooser_multi_t(0, qnumber(widths_), widths_, header_, title) {}
//	private:
//		~chooser_EventInfo() {}
//		const void* get_obj_id(size_t* len) const
//		{
//			*len = strlen(title);
//			return title;
//		}
//		size_t idaapi get_count(void) const
//		{
//			return vec_EventInfo.size();
//		}
//		cbres_t idaapi enter(sizevec_t* sel)
//		{
//			if (sel->size()) {
//				jumpto(vec_EventInfo[sel->at(0)].EventAddr);
//			}
//			return cbres_t();
//		}
//		void idaapi get_row(qstrvec_t* cols_, int* icon_, chooser_item_attrs_t* attrs, size_t n) const
//		{
//			qstrvec_t& cols = *cols_;
//			cols[0].sprnt("%08a", vec_EventInfo[n].EventAddr);
//			acp_utf8(&cols[1], vec_EventInfo[n].ControlName.c_str());
//			acp_utf8(&cols[2], vec_EventInfo[n].EventName.c_str());
//		}
//	};
//
//	qstring title = getUTF8String("控件事件信息");
//	char controlInfoBuf[256];
//	memset(controlInfoBuf,0,sizeof(controlInfoBuf));
//	chooser_EventInfo* pEventWindow = new chooser_EventInfo(title.c_str());
//	for (unsigned int nWindowIndex = 0; nWindowIndex < mVec_GuiInfo.size(); ++nWindowIndex) {
//		for (unsigned int nControlIndex = 0; nControlIndex < mVec_GuiInfo[nWindowIndex].mVec_ControlInfo.size(); ++nControlIndex) {
//			ControlType_t type = GuiParser::GetControlType(mVec_GuiInfo[nWindowIndex].mVec_ControlInfo[nControlIndex].m_controlTypeId);
//			EAppControl* pEAppControl = EAppControlFactory::getEAppControl(type);
//			if (pEAppControl) {
//				for (unsigned int nEventIndex = 0; nEventIndex < mVec_GuiInfo[nWindowIndex].mVec_ControlInfo[nControlIndex].mVec_eventInfo.size(); ++nEventIndex) {
//					eventInfo eEventInfo;
//					eEventInfo.EventAddr = mVec_GuiInfo[nWindowIndex].mVec_ControlInfo[nControlIndex].mVec_eventInfo[nEventIndex].m_EventAddr;
//					eEventInfo.ControlName = mVec_GuiInfo[nWindowIndex].mVec_ControlInfo[nControlIndex].m_controlTypeName;
//					unsigned int index = mVec_GuiInfo[nWindowIndex].mVec_ControlInfo[nControlIndex].mVec_eventInfo[nEventIndex].m_nEventIndex;
//
//					sprintf_s(controlInfoBuf,"_%s_%s", mVec_GuiInfo[nWindowIndex].mVec_ControlInfo[nControlIndex].m_controlName.c_str(), pEAppControl->取事件名称(index).c_str());
//					eEventInfo.EventName = controlInfoBuf;
//					qstring utf8Name;
//					acp_utf8(&utf8Name, eEventInfo.EventName.c_str());
//					set_name(eEventInfo.EventAddr, utf8Name.c_str(), SN_NOWARN);
//					pEventWindow->vec_EventInfo.push_back(eEventInfo);
//				}
//			}
//		}
//	}
//
//	pEventWindow->choose();
//	return 0;
//}

//int GuiParser::MenuHandle_ShowGuiInfo()
//{
//	TWidget* widget = find_widget(getUTF8String("窗口控件信息").c_str());
//
//	if (widget == nullptr) {
//		g_ControlInfoWindow = new ControlInfoWidget();
//		widget = (TWidget*)g_ControlInfoWindow;
//
//		g_ControlInfoWindow->ui.treeWidget->setHeaderLabel(QStringLiteral("易语言窗口"));
//		for (unsigned int nWindowIndex = 0; nWindowIndex < mVec_GuiInfo.size(); ++nWindowIndex) {
//			QTreeWidgetItem* pWindowItem = new QTreeWidgetItem(g_ControlInfoWindow->ui.treeWidget);
//			pWindowItem->setData(0, Qt::UserRole, mVec_GuiInfo[nWindowIndex].m_windowId);
//			pWindowItem->setText(0, QString::asprintf("0x%08X", mVec_GuiInfo[nWindowIndex].m_windowId));
//			qvector<mid_ControlInfo>& vec_ControlInfo = mVec_GuiInfo[nWindowIndex].mVec_ControlInfo;
//			for (unsigned int nControlIndex = 0; nControlIndex < vec_ControlInfo.size(); ++nControlIndex) {
//				QTreeWidgetItem* pControlItem = new QTreeWidgetItem(pWindowItem);
//				EAppControl* pControl= EAppControlFactory::getEAppControl(GuiParser::GetControlType(vec_ControlInfo[nControlIndex].m_controlTypeId));
//				if (pControl) {
//					pControlItem->setIcon(0, QIcon(pControl->取图标资源路径()));
//				}
//				pControlItem->setData(0, Qt::UserRole, vec_ControlInfo[nControlIndex].m_controlId);
//				pControlItem->setText(0, QString::asprintf("0x%08X", vec_ControlInfo[nControlIndex].m_controlId));
//			}
//		}
//		display_widget(widget, WOPN_DP_TAB | WOPN_RESTORE);
//	}
//	else {
//		activate_widget(widget, true);
//	}
//	return 0;
//}


