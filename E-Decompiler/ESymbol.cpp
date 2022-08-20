#include "ESymbol.h"
#include "./Utils/Strings.h"
#include "./Utils/IDAWrapper.h"
#include "TrieTree.h"
#include "SectionManager.h"
#include <ua.hpp>
#include <allins.hpp>
#include "./Utils/Common.h"
#include "./EAppControl/EAppControlFactory.h"
#include "./EAppControl/EAppControl.h"

ESymbol::ESymbol()
{
	int a = 0;
}

ESymbol::~ESymbol()
{
	clearControlData();
}

unsigned int krnln_GetIDSubType(unsigned int ID)
{
	return ID & 0xF0000000;
}

unsigned int krnln_GetIDGroupType(unsigned int ID)
{
	return ID & 0xF000000;
}

//�Ƿ�Ϊ�˵�
bool krnln_IsMenuItemID(unsigned int ID)
{
	return krnln_GetIDGroupType(ID) == 0x6000000 && krnln_GetIDSubType(ID) == 0x20000000;
}

unsigned int GetDataTypeType(unsigned int typeID)
{
	unsigned int result = typeID;
	if (typeID)
	{
		if ((typeID & 0xC0000000) == 0x80000000) {
			result = 1;
		}
		else {
			result = ((typeID & 0xC0000000) != 0x40000000) + 2;
		}
	}
	return result;
}


std::string ESymbol::getControlTypeName(unsigned int typeId)
{
	std::string ret;
	if (GetDataTypeType(typeId) != 3) {
		return ret;
	}

	int libIndex = (typeId >> 0x10) - 1;
	if (libIndex >= vec_ELibInfo.size()) {
		return ret;
	}
	int typeIndex = (unsigned short)typeId - 1;
	if (typeIndex >= vec_ELibInfo[libIndex].mVec_DataTypeInfo.size()) {
		return ret;
	}
	ret = vec_ELibInfo[libIndex].mVec_DataTypeInfo[typeIndex].m_Name;
	return ret;
}


bool ESymbol::LoadEStaticSymbol(unsigned int eHeadAddr, EComHead* eHead)
{
	clearControlData();

	IDAWrapper::show_wait_box(LocalCpToUtf8("����������֧�ֿ�").c_str());
	if (!loadELibInfomation(eHead->lpLibEntry, eHead->dwLibNum)) {
		IDAWrapper::hide_wait_box();
		return false;
	}
	IDAWrapper::hide_wait_box();
	
	eSymbolFuncTypeMap.clear();
	IDAWrapper::show_wait_box(LocalCpToUtf8("ʶ��֧�ֿ⺯��").c_str());
	if (!scanELibFunction(eHead->lpLibEntry, eHead->dwLibNum)) {
		IDAWrapper::hide_wait_box();
		return false;
	}
	IDAWrapper::hide_wait_box();

	scanBasicFunction();

	ea_t dwKrnlEntry = eHead->lpEString;
	if (dwKrnlEntry == 0) {
		dwKrnlEntry = eHead->lpEWindow;
	}

	if (!loadKrnlInterface(dwKrnlEntry)) {
		return false;
	}

	if (eHead->lpEString != 0 && eHead->dwEStringSize != 0) {
		//To do... �����Գ�����Դ��Χ��ʲô����
	}

	if (eHead->lpEWindow != 0 && eHead->dwEWindowSize > 4) {
		IDAWrapper::show_wait_box(LocalCpToUtf8("���������Կؼ���Դ").c_str());
		loadGUIResource(eHead->lpEWindow, eHead->dwEWindowSize);
		hide_wait_box();
		//gMenu_ShowGUIInfo = IDAMenu::CreateMenu(getUTF8String("������/���ڿؼ���Ϣ").c_str(), GuiParser::MenuHandle_ShowGuiInfo);
		//if (GuiParser::GetEventCount()) {
		//	gMenu_ShowEventInfo = IDAMenu::CreateMenu(getUTF8String("������/�ؼ��¼���Ϣ").c_str(), GuiParser::MenuHandle_ShowEventInfo);
		//}
	}

	//if (eHead.dwApiCount) {
	//	show_wait_box(getUTF8String("���������Ե����").c_str());
	//	ImportsParser::ParseUserImports(eHead.dwApiCount, eHead.lpModuleName, eHead.lpApiName);
	//	hide_wait_box();
	//	gMenu_ShowGUIInfo = IDAMenu::CreateMenu(getUTF8String("������/�û������").c_str(), ImportsParser::MenuHandle_ShowImportsInfo);
	//}


	return true;
}

eSymbolFuncType ESymbol::GetFuncSymbolType(unsigned int addr)
{
	return eSymbolFuncTypeMap[addr];
}

EAppControl* ESymbol::GetEAppControl(unsigned int windowID, unsigned int controID)
{
	return allControlMap[eSymbol_ControlIndex(windowID, controID)];
}

bool ESymbol::loadELibInfomation(unsigned int lpLibStartAddr, unsigned int dwLibCount)
{
	if (!dwLibCount || !lpLibStartAddr) {
		return true;
	}


	for (unsigned int nLibIndex = 0; nLibIndex < dwLibCount; ++nLibIndex) {
		unsigned int controlTypeId = 0;
		LIB_INFO tmpLibInfo;
		IDAWrapper::get_bytes(&tmpLibInfo, sizeof(LIB_INFO), IDAWrapper::get_dword(lpLibStartAddr));
		lpLibStartAddr = lpLibStartAddr + 4;

		//�ж��Ƿ����֧�ֿ��ʽ
		if (tmpLibInfo.m_dwLibFormatVer != 0x1312D65) {
			continue;
		}

		eSymbol_ELibInfo eLibInfo;
		eLibInfo.m_Name = IDAWrapper::get_shortstring(tmpLibInfo.m_lpName);
		eLibInfo.m_Guid = IDAWrapper::get_shortstring(tmpLibInfo.m_lpGuid);
		eLibInfo.m_nMajorVersion = tmpLibInfo.m_nMajorVersion;
		eLibInfo.m_nMinorVersion = tmpLibInfo.m_nMinorVersion;

		unsigned int lpFirstDataType = tmpLibInfo.m_lpDataType;
		for (int nDataTypeIndex = 0; nDataTypeIndex < tmpLibInfo.m_nDataTypeCount; ++nDataTypeIndex) {
			LIB_DATA_TYPE_INFO tmpDataTypeInfo;
			IDAWrapper::get_bytes(&tmpDataTypeInfo, sizeof(LIB_DATA_TYPE_INFO), lpFirstDataType);
			lpFirstDataType += sizeof(LIB_DATA_TYPE_INFO);

			eSymbol_EDataTypeInfo eDataType;
			if (tmpDataTypeInfo.m_lpszName) {
				controlTypeId = (nLibIndex + 1) << 0x10;
				controlTypeId = controlTypeId + (nDataTypeIndex + 1);
				eDataType.m_Name = IDAWrapper::get_shortstring(tmpDataTypeInfo.m_lpszName);
				EAppControlFactory::Instance().RegisterEAppControl(eLibInfo.m_Guid + eDataType.m_Name,controlTypeId);
			}
			eLibInfo.mVec_DataTypeInfo.push_back(eDataType);
		}
		vec_ELibInfo.push_back(eLibInfo);
	}
	return true;
}

bool ESymbol::scanELibFunction(unsigned int lpLibStartAddr, unsigned int dwLibCount)
{
	for (unsigned int nLibIndex = 0; nLibIndex < dwLibCount; ++nLibIndex) {
		LIB_INFO tmpLibInfo;
		IDAWrapper::get_bytes(&tmpLibInfo, sizeof(LIB_INFO), IDAWrapper::get_dword(lpLibStartAddr));
		lpLibStartAddr = lpLibStartAddr + 4;

		//�ж��Ƿ����֧�ֿ��ʽ
		if (tmpLibInfo.m_dwLibFormatVer != 0x1312D65) {
			continue;
		}

		//֧�ֿ�ȱ�ٺ���
		if (!tmpLibInfo.m_nCmdCount || !tmpLibInfo.m_lpCmdsFunc) {
			continue;
		}
	
		char LibPath[256] = { 0 };
		sprintf_s(LibPath, "%s\\esig\\%s.esig", IDAWrapper::idadir("plugins"), IDAWrapper::get_shortstring(tmpLibInfo.m_lpName).c_str());
		
		TrieTree ESIGTREE;
		if (!ESIGTREE.LoadSig(LibPath)) {
			continue;
		}
	
		uint32* pFuncBuf = (uint32*)qalloc(tmpLibInfo.m_nCmdCount * 4);
		IDAWrapper::get_bytes(pFuncBuf, tmpLibInfo.m_nCmdCount * 4, tmpLibInfo.m_lpCmdsFunc);
		for (unsigned int nFuncIndex = 0; nFuncIndex < tmpLibInfo.m_nCmdCount; ++nFuncIndex) {
			unsigned int funcAddr = pFuncBuf[nFuncIndex];

#ifdef _DEBUG
			if (funcAddr == 0x0049B790) {
				int a = 0;
			}
#endif
			eSymbolFuncTypeMap[funcAddr] = eFunc_KrnlLibFunc;
			add_func(funcAddr);
			char* pFuncName = ESIGTREE.MatchFunc(SectionManager::LinearAddrToVirtualAddr(funcAddr));
			if (!pFuncName) {
				//qstring errorFuncName;
				IDAWrapper::msg("[ScanLibFunction]Match Function Error,%a\n", funcAddr);
				//errorFuncName.sprnt("δ֪����_%a", funcAddr);
				//IDAWrapper::setFuncName(funcAddr, errorFuncName.c_str());
				continue;
			}
			IDAWrapper::setFuncName(funcAddr,pFuncName);
		}
		qfree(pFuncBuf);
	}
	return true;
}

bool ESymbol::scanBasicFunction()
{
	TrieTree BASICTREE;
	qstring LibPath;
	LibPath.sprnt("%s\\esig\\�����Ի�������.esig", IDAWrapper::idadir("plugin"));

	std::map<ea_t, qstring> mMap_BasicFunc;
	if (!BASICTREE.LoadSig(LibPath.c_str())) {
		return false;
	}
	for (unsigned int idx = 0; idx < get_func_qty(); ++idx) {
		func_t* pFunc = getn_func(idx);
		if (!pFunc) {
			continue;
		}
		if (eSymbolFuncTypeMap[pFunc->start_ea] != 0x0) {
			continue;
		}
		qstring funcName = BASICTREE.MatchFunc(SectionManager::LinearAddrToVirtualAddr(pFunc->start_ea));
		if (!funcName.empty()) {
			mMap_BasicFunc[pFunc->start_ea] = funcName;
			IDAWrapper::setFuncName(pFunc->start_ea, funcName.c_str());
		}
		if (funcName == "�ı����") {
			IDAWrapper::apply_cdecl(pFunc->start_ea, "char* __usercall strcat@<eax>(int argCount@<ecx>, ...);");
			eSymbolFuncTypeMap[pFunc->start_ea] = eFunc_Strcat;
		}
	}
	return true;
}

bool ESymbol::loadKrnlInterface(unsigned int lpKrnlEntry)
{
	qstring jmpOtherHelpHex;
	jmpOtherHelpHex.sprnt("FF 25 %08X", lpKrnlEntry - 4);

	lpKrnlEntry -= sizeof(eSymbol_KrnlCall);
	get_bytes(&this->krnlCall, sizeof(eSymbol_KrnlCall), lpKrnlEntry);

	this->krnlJmp.Jmp_MOtherHelp = SectionManager::SeachBin(jmpOtherHelpHex);
	if (this->krnlJmp.Jmp_MOtherHelp == BADADDR) {
		return false;
	}

	ea_t aboveAddr = krnlJmp.Jmp_MOtherHelp;
	do
	{
		insn_t tmpIns;
		aboveAddr = decode_prev_insn(&tmpIns, aboveAddr);
		if (aboveAddr == BADADDR) {
			break;
		}
		if (tmpIns.itype != NN_jmpni || tmpIns.ops[0].type != o_mem) {
			break;
		}
		ea_t CallAddr = ReadUInt(SectionManager::LinearAddrToVirtualAddr(tmpIns.ops[0].addr));
		if (!registerKrnlJmpAddr(CallAddr, aboveAddr)) {
			return false;
		}
	} while (true);

	ea_t belowAddr = krnlJmp.Jmp_MOtherHelp;
	do
	{
		insn_t tmpIns;
		int len = decode_insn(&tmpIns, belowAddr);
		if (!len) {
			return false;
		}
		if (tmpIns.itype != NN_jmpni || tmpIns.ops[0].type != o_mem) {
			break;
		}
		ea_t CallAddr = ReadUInt(SectionManager::LinearAddrToVirtualAddr(tmpIns.ops[0].addr));
		registerKrnlJmpAddr(CallAddr, belowAddr);
		belowAddr += tmpIns.size;
	} while (true);

	IDAWrapper::setFuncName(krnlJmp.Jmp_MReportError, "����ص�");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MCallDllCmd, "DLL����");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MCallLibCmd, "����֧�ֿ�����");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MCallKrnlLibCmd, "����֧�ֿ�����");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MReadProperty, "��ȡ�������");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MWriteProperty, "�����������");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MMalloc, "�����ڴ�");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MRealloc, "���·����ڴ�");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MFree, "�ͷ��ڴ�");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MExitProcess, "����");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MMessageLoop, "������Ϣѭ��");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MLoadBeginWin, "������������");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MOtherHelp, "��������");

	eSymbolFuncTypeMap[krnlJmp.Jmp_MReadProperty] = eFunc_KrnlReadProerty;
	eSymbolFuncTypeMap[krnlJmp.Jmp_MWriteProperty] = eFunc_KrnlWriteProperty;

	IDAWrapper::apply_cdecl(krnlJmp.Jmp_MCallDllCmd, "krnlRet __usercall CallDllCmd@<eax:edx>(unsigned int index@<eax>,...);");
	IDAWrapper::apply_cdecl(krnlJmp.Jmp_MCallLibCmd, "krnlRet __usercall CallLibCmd@<eax:edx>(unsigned int libFunc@<ebx>, int argCount, ...);");
	IDAWrapper::apply_cdecl(krnlJmp.Jmp_MCallKrnlLibCmd, "krnlRet __usercall CallKrnlLibCmd@<eax:edx>(unsigned int libFunc@<ebx>, int argCount, ...);");
	IDAWrapper::apply_cdecl(krnlJmp.Jmp_MReadProperty, "krnlRet __usercall CallReadProperty@<eax:edx>(unsigned int,unsigned int,unsigned int,unsigned int);");
	IDAWrapper::apply_cdecl(krnlJmp.Jmp_MWriteProperty, "void __cdecl CallWriteProperty(unsigned int windowId,unsigned int controlId,unsigned int nPropertyIndex,unsigned int nDataSize,UNIT_PROPERTY_VALUE,unsigned int ppszTipText);");
	IDAWrapper::apply_cdecl(krnlJmp.Jmp_MFree, "void __cdecl CallFree(LPVOID lpMem);");

	return true;
}

void ESymbol::parseControlBasciProperty(unsigned char* lpControlInfo,EAppControl* outControl)
{
	//�����ַ���1?
	ReadStr(lpControlInfo);
	lpControlInfo += qstrlen(lpControlInfo) + 1;

	//�洢����?
	ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	unsigned int m_left = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	unsigned int m_top = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	unsigned int m_width = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	unsigned int m_height = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	//ֵΪ0,�����洢LoadCursorA���صľ��ֵ��
	unsigned int hCURSOR = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	//���ؼ�ID
	unsigned int fatherControlId = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	//�ӿؼ���Ŀ
	unsigned int childControlCount = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	for (unsigned int n = 0; n < childControlCount; ++n) {
		unsigned int tmpChildControlId = ReadUInt(lpControlInfo);
		lpControlInfo += 4;
		//out_Property.mVec_childControl.push_back(tmpChildControlId);
	}

	//δ֪ƫ��
	unsigned int offset2 = ReadUInt(lpControlInfo);
	lpControlInfo += offset2 + 4;

	//���
	std::string m_tag = ReadStr(lpControlInfo);
	lpControlInfo += qstrlen(lpControlInfo) + 1;

	//δ֪��ֵ
	lpControlInfo += 12;

	int dwEventCount = ReadInt(lpControlInfo);
	lpControlInfo += 4;

	for (int nIndexEvent = 0; nIndexEvent < dwEventCount; ++nIndexEvent) {
		eSymbol_EventInfo tmpEvent;
		tmpEvent.eventIndex = ReadInt(lpControlInfo);
		lpControlInfo += 4;
		tmpEvent.eventAddr = ReadUInt(lpControlInfo) + userCodeStartAddr;
		lpControlInfo += 4;
		outControl->eventList.push_back(tmpEvent);
	}
	return;
}

bool ESymbol::loadGUIResource(unsigned int lpGUIStart, unsigned int infoSize)
{
	std::vector<unsigned char> tmpGuiBuf;
	tmpGuiBuf.resize(infoSize);
	IDAWrapper::get_bytes(&tmpGuiBuf[0], infoSize, lpGUIStart);

	//��ǰ������ַ
	unsigned char* lpCurrentParseAddr = &tmpGuiBuf[0];

	std::vector<unsigned int> vec_WindowId;
	unsigned int dwTotalWindowCount = ReadUInt(&tmpGuiBuf[0]) >> 3;
	lpCurrentParseAddr += 4;

	for (unsigned int n = 0; n < dwTotalWindowCount; ++n) {
		vec_WindowId.push_back(ReadUInt(lpCurrentParseAddr));
		lpCurrentParseAddr += 4;
	}

	//����������ֵ?
	for (unsigned int n = 0; n < dwTotalWindowCount; ++n) {
		//uint32 unknowId = ReadUInt(lpCurrentParseAddr);
		lpCurrentParseAddr += 4;
	}

	char sprinfBuf[1024] = { 0 };
	for (unsigned int nIndexWindow = 0; nIndexWindow < dwTotalWindowCount; ++nIndexWindow) {
		unsigned char* lpWindowInfo = lpCurrentParseAddr;

		eSymbol_GuiInfo eGuiInfo;
		eGuiInfo.windowId = vec_WindowId[nIndexWindow];

		//��ʱδ֪
		uint32 unKnownFieldA = ReadUInt(lpWindowInfo);
		lpWindowInfo += 4;
		uint32 unKnownFieldB = ReadUInt(lpWindowInfo);
		lpWindowInfo += 4;

		//��������������CString,��Ϊ��
		lpWindowInfo += 8;

		//���������еĿؼ��ܸ���
		uint32 dwTotalControlCount = ReadUInt(lpWindowInfo);
		lpWindowInfo += 4;

		//���������еĿؼ���ռ�ô�С
		uint32 dwTotalControlSize = ReadUInt(lpWindowInfo);
		lpWindowInfo += 4;

		//��ʼ�����ؼ�
		unsigned char* lpControlArray = lpWindowInfo;
		{
			//�����ؼ�ID,����0x160612BC
			std::vector<unsigned int> vec_ControlId;
			for (unsigned int j = 0; j < dwTotalControlCount; ++j) {
				vec_ControlId.push_back(ReadUInt(lpControlArray));
				lpControlArray += 4;
			}

			//�����ؼ�ƫ��
			std::vector<unsigned int> vec_ControlOffset;
			for (unsigned int j = 0; j < dwTotalControlCount; ++j) {
				vec_ControlOffset.push_back(ReadUInt(lpControlArray));
				lpControlArray += 4;
			}

			//�����ؼ�����
			for (unsigned int nIndexControl = 0; nIndexControl < dwTotalControlCount; ++nIndexControl) {
				unsigned char* lpControlInfo = lpControlArray + vec_ControlOffset[nIndexControl];

				//�ؼ�ռ�õĴ�С
				int32 dwControlSize = ReadInt(lpControlInfo);
				lpControlInfo += 4;

				unsigned int propertyAddr = lpGUIStart + (lpControlInfo - &tmpGuiBuf[0]);
				unsigned int propertySize = dwControlSize;

				unsigned int dwControlTypeId = ReadUInt(lpControlInfo);
				lpControlInfo += 4;

				//�̶���20�����ֽ�,����ʹ��?
				lpControlInfo += 20;

				EAppControl* eControlInfo = EAppControlFactory::Instance().CreateEAppControl(dwControlTypeId);
				if (!eControlInfo) {
					continue;
				}
				if (dwControlTypeId == 0x10001) {
					eControlInfo->controlName = ReadStr(lpControlInfo);
					lpControlInfo += strlen((const char*)lpControlInfo) + 1;
					if (eControlInfo->controlName.empty()) {
						sprintf_s(sprinfBuf, "����0x%08X", eGuiInfo.windowId);
						eControlInfo->controlName = sprinfBuf;
					}
					parseControlBasciProperty(lpControlInfo, eControlInfo);
				}
				else if (krnln_IsMenuItemID(vec_ControlId[nIndexControl])) {
					lpControlInfo += 14;
					eControlInfo->controlName = ReadStr(lpControlInfo);
				}
				else {
					eControlInfo->controlName = ReadStr(lpControlInfo);
					lpControlInfo += qstrlen(lpControlInfo) + 1;
					parseControlBasciProperty(lpControlInfo, eControlInfo);
				}
				eControlInfo->controlId = vec_ControlId[nIndexControl];
				eControlInfo->controlTypeId = dwControlTypeId;
				eControlInfo->controlTypeName = getControlTypeName(dwControlTypeId);
				
				allControlMap[eSymbol_ControlIndex(eGuiInfo.windowId, eControlInfo->controlId)] = eControlInfo;
				eGuiInfo.controlInfoList.push_back(eControlInfo);
			}
		}

		vec_GuiInfo.push_back(eGuiInfo);
		lpCurrentParseAddr = lpWindowInfo + dwTotalControlSize;
	}

	return true;
}

//void ESymbol::parseControlBasciProperty(unsigned char* lpControlInfo, eSymbol_ControlInfo& out_Property)
//{
//	//�����ַ���1?
//	ReadStr(lpControlInfo);
//	lpControlInfo += qstrlen(lpControlInfo) + 1;
//
//	//�洢����?
//	ReadUInt(lpControlInfo);
//	lpControlInfo += 4;
//
//	unsigned int m_left = ReadUInt(lpControlInfo);
//	lpControlInfo += 4;
//
//	unsigned int m_top = ReadUInt(lpControlInfo);
//	lpControlInfo += 4;
//
//	unsigned int m_width = ReadUInt(lpControlInfo);
//	lpControlInfo += 4;
//
//	unsigned int m_height = ReadUInt(lpControlInfo);
//	lpControlInfo += 4;
//
//	//ֵΪ0,�����洢LoadCursorA���صľ��ֵ��
//	unsigned int hCURSOR = ReadUInt(lpControlInfo);
//	lpControlInfo += 4;
//
//	//���ؼ�ID
//	unsigned int fatherControlId = ReadUInt(lpControlInfo);
//	lpControlInfo += 4;
//
//	//�ӿؼ���Ŀ
//	unsigned int childControlCount = ReadUInt(lpControlInfo);
//	lpControlInfo += 4;
//
//	for (unsigned int n = 0; n < childControlCount; ++n) {
//		unsigned int tmpChildControlId = ReadUInt(lpControlInfo);
//		lpControlInfo += 4;
//		//out_Property.mVec_childControl.push_back(tmpChildControlId);
//	}
//
//	//δ֪ƫ��
//	unsigned int offset2 = ReadUInt(lpControlInfo);
//	lpControlInfo += offset2 + 4;
//
//	//���
//	std::string m_tag = ReadStr(lpControlInfo);
//	lpControlInfo += qstrlen(lpControlInfo) + 1;
//
//	//δ֪��ֵ
//	lpControlInfo += 12;
//
//	int dwEventCount = ReadInt(lpControlInfo);
//	lpControlInfo += 4;
//
//	for (int nIndexEvent = 0; nIndexEvent < dwEventCount; ++nIndexEvent) {
//		eSymbol_EventInfo tmpEvent;
//		tmpEvent.eventIndex = ReadInt(lpControlInfo);
//		lpControlInfo += 4;
//		tmpEvent.eventAddr = ReadUInt(lpControlInfo) + this->userCodeStartAddr;
//		lpControlInfo += 4;
//		out_Property.eventList.push_back(tmpEvent);
//	}
//}


bool ESymbol::registerKrnlJmpAddr(unsigned int callAddr, unsigned int setAddr)
{
	if (callAddr == krnlCall.krnl_MReportError) {
		krnlJmp.Jmp_MReportError = setAddr;
	}
	else if (callAddr == krnlCall.krnl_MCallDllCmd) {
		krnlJmp.Jmp_MCallDllCmd = setAddr;
	}
	else if (callAddr == krnlCall.krnl_MCallLibCmd) {
		krnlJmp.Jmp_MCallLibCmd = setAddr;
	}
	else if (callAddr == krnlCall.krnl_MCallKrnlLibCmd) {
		krnlJmp.Jmp_MCallKrnlLibCmd = setAddr;
	}
	else if (callAddr == krnlCall.krnl_MReadProperty) {
		krnlJmp.Jmp_MReadProperty = setAddr;
	}
	else if (callAddr == krnlCall.krnl_MWriteProperty) {
		krnlJmp.Jmp_MWriteProperty = setAddr;
	}
	else if (callAddr == krnlCall.krnl_MMalloc) {
		krnlJmp.Jmp_MMalloc = setAddr;
	}
	else if (callAddr == krnlCall.krnl_MRealloc) {
		krnlJmp.Jmp_MRealloc = setAddr;
	}
	else if (callAddr == krnlCall.krnl_MFree) {
		krnlJmp.Jmp_MFree = setAddr;
	}
	else if (callAddr == krnlCall.krnl_MExitProcess) {
		krnlJmp.Jmp_MExitProcess = setAddr;
	}
	else if (callAddr == krnlCall.krnl_MMessageLoop) {
		krnlJmp.Jmp_MMessageLoop = setAddr;
	}
	else if (callAddr == krnlCall.krnl_MLoadBeginWin) {
		krnlJmp.Jmp_MLoadBeginWin = setAddr;
	}
	else if (callAddr == krnlCall.krnl_MOtherHelp) {
		krnlJmp.Jmp_MOtherHelp = setAddr;
	}
	else {
		return false;
	}
	return true;
}

void ESymbol::clearControlData()
{
	allControlMap.clear();
	for (unsigned int nWindowIndex = 0; nWindowIndex < vec_GuiInfo.size(); ++nWindowIndex) {
		for (unsigned int nControlIndex = 0; nControlIndex < vec_GuiInfo[nWindowIndex].controlInfoList.size(); ++nControlIndex) {
			EAppControl* eAppControl = vec_GuiInfo[nWindowIndex].controlInfoList[nControlIndex];
			if (eAppControl) {
				delete eAppControl;
			}
		}
	}
	vec_GuiInfo.clear();
}