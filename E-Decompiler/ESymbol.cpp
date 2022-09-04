#include "ESymbol.h"
#include "./Utils/Strings.h"
#include "./Utils/IDAWrapper.h"
#include "TrieTree.h"
#include "SectionManager.h"
#include <ua.hpp>
#include <allins.hpp>
#include <struct.hpp>
#include <typeinf.hpp>
#include <unordered_set>
#include <hexrays.hpp>
#include "./Utils/Common.h"
#include "./Utils/IDAMenu.h"
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

//是否为菜单
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

	IDAWrapper::show_wait_box(LocalCpToUtf8("解析易语言支持库").c_str());
	if (!loadELibInfomation(eHead->lpLibEntry, eHead->dwLibNum)) {
		IDAWrapper::hide_wait_box();
		return false;
	}
	IDAWrapper::hide_wait_box();
	
	eSymbolFuncTypeMap.clear();
	IDAWrapper::show_wait_box(LocalCpToUtf8("识别支持库函数").c_str());
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

	IDAWrapper::show_wait_box(LocalCpToUtf8("扫描易语言类").c_str());
	scanEClassTable();
	IDAWrapper::hide_wait_box();

	if (eHead->lpEString != 0 && eHead->dwEStringSize != 0) {
		//To do... 易语言常量资源范围有什么用呢
	}

	if (eHead->lpEWindow != 0 && eHead->dwEWindowSize > 4) {
		IDAWrapper::show_wait_box(LocalCpToUtf8("解析易语言控件资源").c_str());
		loadGUIResource(eHead->lpEWindow, eHead->dwEWindowSize);
		hide_wait_box();
	}

	if (eHead->dwApiCount) {
		IDAWrapper::show_wait_box(LocalCpToUtf8("解析易语言导入表").c_str());
		loadUserImports(eHead->dwApiCount, eHead->lpModuleName, eHead->lpApiName);
		hide_wait_box();
	}
	
	setGuiEventName();
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

		//判断是否符合支持库格式
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

		//判断是否符合支持库格式
		if (tmpLibInfo.m_dwLibFormatVer != 0x1312D65) {
			continue;
		}

		//支持库缺少函数
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
				//errorFuncName.sprnt("未知命令_%a", funcAddr);
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
	LibPath.sprnt("%s\\esig\\易语言基础命令.esig", IDAWrapper::idadir("plugins"));

	std::map<ea_t, qstring> mMap_BasicFunc;
	if (!BASICTREE.LoadSig(LibPath.c_str())) {
		return false;
	}
	for (unsigned int idx = 0; idx < get_func_qty(); ++idx) {
		func_t* pFunc = getn_func(idx);
		if (!pFunc) {
			continue;
		}
		if (pFunc->start_ea >= userCodeEndAddr) {
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
		if (funcName == "文本相加") {
			IDAWrapper::apply_cdecl(pFunc->start_ea, "char* __usercall strcat@<eax>(int argCount@<ecx>, ...);");
			eSymbolFuncTypeMap[pFunc->start_ea] = eFunc_Strcat;
		}
		else if (funcName == "连续省略参数") {
			IDAWrapper::apply_cdecl(pFunc->start_ea, "void __usercall pushDefaultParam(int argCount@<ebx>);");
			eSymbolFuncTypeMap[pFunc->start_ea] = eFunc_PushDefaultArg;
			handleFuncPushDefaultArg(pFunc->start_ea);
		}
		else if (funcName == "文本比较") {
			IDAWrapper::apply_cdecl(pFunc->start_ea, "int __cdecl strcmp(char* _Str1,char* _Str2);");
		}
		else if (funcName == "计算多维数组下标") {
			IDAWrapper::apply_cdecl(pFunc->start_ea, "int __usercall addMutilVecIndex@<edx>(int index@<eax>,int dim@<ecx>,DWORD* pBounds);");
			eSymbolFuncTypeMap[pFunc->start_ea] = eFunc_CalMultiArrayIndex;
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

	IDAWrapper::setFuncName(krnlJmp.Jmp_MReportError, "错误回调");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MCallDllCmd, "DLL命令");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MCallLibCmd, "三方支持库命令");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MCallKrnlLibCmd, "核心支持库命令");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MReadProperty, "读取组件属性");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MWriteProperty, "设置组件属性");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MMalloc, "分配内存");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MRealloc, "重新分配内存");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MFree, "释放内存");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MExitProcess, "结束");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MMessageLoop, "窗口消息循环");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MLoadBeginWin, "载入启动窗口");
	IDAWrapper::setFuncName(krnlJmp.Jmp_MOtherHelp, "辅助函数");

	eSymbolFuncTypeMap[krnlJmp.Jmp_MReadProperty] = eFunc_KrnlReadProerty;
	eSymbolFuncTypeMap[krnlJmp.Jmp_MWriteProperty] = eFunc_KrnlWriteProperty;
	eSymbolFuncTypeMap[krnlJmp.Jmp_MCallKrnlLibCmd] = eFunc_KrnlLibFunc;
	eSymbolFuncTypeMap[krnlJmp.Jmp_MCallDllCmd] = eFunc_KrnlDllCmd;
	eSymbolFuncTypeMap[krnlJmp.Jmp_MReportError] = eFunc_KrnlReportError;
	eSymbolFuncTypeMap[krnlJmp.Jmp_MFree] = eFunc_KrnlFreeMem;

	IDAWrapper::apply_cdecl(krnlJmp.Jmp_MOtherHelp, "krnlRet __usercall CallOtherHelp@<eax:edx>(unsigned int index@<eax>,...);");
	IDAWrapper::apply_cdecl(krnlJmp.Jmp_MCallDllCmd, "krnlRet __usercall CallDllCmd@<eax:edx>(unsigned int index@<eax>,...);");
	IDAWrapper::apply_cdecl(krnlJmp.Jmp_MCallLibCmd, "krnlRet __usercall CallLibCmd@<eax:edx>(unsigned int libFunc@<ebx>, int argCount, ...);");
	IDAWrapper::apply_cdecl(krnlJmp.Jmp_MCallKrnlLibCmd, "krnlRet __usercall CallKrnlLibCmd@<eax:edx>(unsigned int libFunc@<ebx>, int argCount, ...);");
	IDAWrapper::apply_cdecl(krnlJmp.Jmp_MReadProperty, "krnlRet __usercall CallReadProperty@<eax:edx>(unsigned int,unsigned int,unsigned int,unsigned int);");
	IDAWrapper::apply_cdecl(krnlJmp.Jmp_MWriteProperty, "void __cdecl CallWriteProperty(unsigned int windowId,unsigned int controlId,unsigned int nPropertyIndex,unsigned int nDataSize,UNIT_PROPERTY_VALUE,unsigned int ppszTipText);");
	IDAWrapper::apply_cdecl(krnlJmp.Jmp_MFree, "void __cdecl CallFree(LPVOID lpMem);");
	IDAWrapper::apply_cdecl(krnlJmp.Jmp_MMalloc, "LPVOID __cdecl malloc(SIZE_T dwBytes);");
	return true;
}

void ESymbol::parseControlBasciProperty(unsigned char* lpControlInfo,EAppControl* outControl)
{
	//无用字符串1?
	ReadStr(lpControlInfo);
	lpControlInfo += qstrlen(lpControlInfo) + 1;

	//存储数据?
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

	//值为0,用来存储LoadCursorA返回的句柄值的
	unsigned int hCURSOR = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	//父控件ID
	unsigned int fatherControlId = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	//子控件数目
	unsigned int childControlCount = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	for (unsigned int n = 0; n < childControlCount; ++n) {
		unsigned int tmpChildControlId = ReadUInt(lpControlInfo);
		lpControlInfo += 4;
		//out_Property.mVec_childControl.push_back(tmpChildControlId);
	}

	//未知偏移
	unsigned int offset2 = ReadUInt(lpControlInfo);
	lpControlInfo += offset2 + 4;

	//标记
	std::string m_tag = ReadStr(lpControlInfo);
	lpControlInfo += qstrlen(lpControlInfo) + 1;

	//未知的值
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

	//当前解析地址
	unsigned char* lpCurrentParseAddr = &tmpGuiBuf[0];

	std::vector<unsigned int> vec_WindowId;
	unsigned int dwTotalWindowCount = ReadUInt(&tmpGuiBuf[0]) >> 3;
	lpCurrentParseAddr += 4;

	for (unsigned int n = 0; n < dwTotalWindowCount; ++n) {
		vec_WindowId.push_back(ReadUInt(lpCurrentParseAddr));
		lpCurrentParseAddr += 4;
	}

	//编译器遗留值?
	for (unsigned int n = 0; n < dwTotalWindowCount; ++n) {
		//uint32 unknowId = ReadUInt(lpCurrentParseAddr);
		lpCurrentParseAddr += 4;
	}

	char sprinfBuf[1024] = { 0 };
	for (unsigned int nIndexWindow = 0; nIndexWindow < dwTotalWindowCount; ++nIndexWindow) {
		unsigned char* lpWindowInfo = lpCurrentParseAddr;

		//暂时未知
		uint32 unKnownFieldA = ReadUInt(lpWindowInfo);
		lpWindowInfo += 4;
		uint32 unKnownFieldB = ReadUInt(lpWindowInfo);
		lpWindowInfo += 4;

		//接下来跟着两个CString,都为空
		lpWindowInfo += 8;

		//单个窗口中的控件总个数
		uint32 dwTotalControlCount = ReadUInt(lpWindowInfo);
		lpWindowInfo += 4;

		//单个窗口中的控件总占用大小
		uint32 dwTotalControlSize = ReadUInt(lpWindowInfo);
		lpWindowInfo += 4;

		//开始解析控件
		unsigned char* lpControlArray = lpWindowInfo;
		{
			//解析控件ID,例如0x160612BC
			std::vector<unsigned int> vec_ControlId;
			for (unsigned int j = 0; j < dwTotalControlCount; ++j) {
				vec_ControlId.push_back(ReadUInt(lpControlArray));
				lpControlArray += 4;
			}

			//解析控件偏移
			std::vector<unsigned int> vec_ControlOffset;
			for (unsigned int j = 0; j < dwTotalControlCount; ++j) {
				vec_ControlOffset.push_back(ReadUInt(lpControlArray));
				lpControlArray += 4;
			}

			//解析控件属性
			for (unsigned int nIndexControl = 0; nIndexControl < dwTotalControlCount; ++nIndexControl) {
				unsigned char* lpControlInfo = lpControlArray + vec_ControlOffset[nIndexControl];

				//控件占用的大小
				int32 dwControlSize = ReadInt(lpControlInfo);
				lpControlInfo += 4;

				unsigned int propertyAddr = lpGUIStart + (lpControlInfo - &tmpGuiBuf[0]);
				unsigned int propertySize = dwControlSize;

				unsigned int dwControlTypeId = ReadUInt(lpControlInfo);
				lpControlInfo += 4;

				//固定的20个空字节,保留使用?
				lpControlInfo += 20;

				EAppControl* eControlInfo = EAppControlFactory::Instance().CreateEAppControl(dwControlTypeId);
				if (!eControlInfo) {
					continue;
				}
				eControlInfo->windowID = vec_WindowId[nIndexWindow];
		
				if (dwControlTypeId == 0x10001) {
					eControlInfo->controlName = ReadStr(lpControlInfo);
					lpControlInfo += strlen((const char*)lpControlInfo) + 1;
					if (eControlInfo->controlName.empty()) {
						sprintf_s(sprinfBuf, "窗口0x%08X", eControlInfo->windowID);
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
				
				allControlMap[eSymbol_ControlIndex(eControlInfo->windowID, eControlInfo->controlId)] = eControlInfo;
				allControlList.push_back(eControlInfo);
			}
		}
		lpCurrentParseAddr = lpWindowInfo + dwTotalControlSize;
	}

	return true;
}

bool ESymbol::loadUserImports(unsigned int dwApiCount, unsigned int lpModuleName, unsigned int lpApiName)
{
	importsApiList.clear();
	tmpImportsApiList.clear();

	unsigned char* pszLibnameAddr = SectionManager::LinearAddrToVirtualAddr(lpModuleName);
	unsigned char* pszApinameAddr = SectionManager::LinearAddrToVirtualAddr(lpApiName);
	for (unsigned int n = 0; n < dwApiCount; ++n) {
		char* pszLibname = (char*)SectionManager::LinearAddrToVirtualAddr(ReadUInt(pszLibnameAddr));
		char* pszApiname = (char*)SectionManager::LinearAddrToVirtualAddr(ReadUInt(pszApinameAddr));
		eSymbol_ImportsApi eImportsApi;
		eImportsApi.libName = pszLibname;
		eImportsApi.apiName = pszApiname;
		importsApiList.push_back(eImportsApi);

		int iIndex = eImportsApi.libName.rfind('.');
		if (iIndex != -1) {
			eImportsApi.libName = eImportsApi.libName.substr(0, iIndex);
		}
		if (!eImportsApi.libName.empty()) {
			eImportsApi.apiName = eImportsApi.libName + "." + eImportsApi.apiName;
		}
		tmpImportsApiList.push_back(LocalCpToUtf8(eImportsApi.apiName.c_str()));
		pszLibnameAddr += 4;
		pszApinameAddr += 4;
	}

	return true;
}

bool ESymbol::handleFuncPushDefaultArg(unsigned int callAddr)
{
	std::vector<unsigned int> xRefList = IDAWrapper::getAllCodeXrefAddr(callAddr);
	for (unsigned int n = 0; n < xRefList.size(); ++n) {
		insn_t tmpIns;
		if (decode_prev_insn(&tmpIns, xRefList[n]) == BADADDR) {
			continue;
		}
		//mov ebx,xxx
		if (tmpIns.itype != NN_mov || tmpIns.ops[0].reg != 0x3) {
			continue;
		}
		unsigned int argCount = tmpIns.ops[1].value;
		IDAWrapper::add_user_stkpnt(xRefList[n] + 5, -(argCount * 4));
	}
	return true;
}

bool ESymbol::scanEClassTable()
{
	//不信有区段大于0x1000000
	qstring movClassTable;
	movClassTable.sprnt("C7 03 ?? ?? ?? %02X", userCodeStartAddr >> 0x18);

	compiled_binpat_vec_t binPat;
	parse_binpat_str(&binPat, 0x0, movClassTable.c_str(), 16);

	ea_t searchStartAddr = userCodeStartAddr;
	std::unordered_set<unsigned int> guessClassTableList;
	while (true)
	{
		searchStartAddr = bin_search2(searchStartAddr, userCodeEndAddr, binPat, 0x0);
		if (searchStartAddr == BADADDR)
		{
			break;
		}
		auto guessAddr = IDAWrapper::get_dword(searchStartAddr + 0x2);
		searchStartAddr = searchStartAddr + 6;
		if (guessAddr < userCodeStartAddr) {
			continue;
		}
		//判断虚表中是否含有拷贝函数,有待验证
		auto copyHead = IDAWrapper::get_word(IDAWrapper::get_dword(guessAddr + 0x4));
		if (copyHead != 0x6850) {
			continue;
		}
		guessClassTableList.insert(guessAddr);
	}

	if (guessClassTableList.size() == 0) {
		return true;
	}

	//根本无法确定虚表的大小,因此尽可能扩大
	auto idati = (til_t*)get_idati();
	for (std::unordered_set<unsigned int>::iterator it = guessClassTableList.begin(); it != guessClassTableList.end(); ++it) {
		std::vector<unsigned int> vTable;
		unsigned int vftPtr = *it;
		vTable.push_back(IDAWrapper::get_dword(vftPtr));
		vTable.push_back(IDAWrapper::get_dword(vftPtr + 4));
		vftPtr = vftPtr + 8;
		do
		{
			if (guessClassTableList.count(vftPtr)) {
				break;
			}
			auto guessFunc = IDAWrapper::get_dword(vftPtr);
			if (guessFunc <= userCodeStartAddr || guessFunc >= userCodeEndAddr) {
				break;
			}
			vTable.push_back(guessFunc);
			vftPtr = vftPtr + 4;
		} while (true);

		qstring vTableName;
		vTableName.sprnt("vtable_%a",*it);
		tid_t idStruct = add_struc(-1, vTableName.c_str(),false);
		struc_t* pStruct = get_struc(idStruct);
		if (!pStruct) {
			continue;
		}
		qstring fieldName;
		for (unsigned int n = 0; n < vTable.size(); ++n) {
			get_func_name(&fieldName, vTable[n]);
			if (fieldName.empty()) {
				fieldName.sprnt("sub_%a", vTable[n]);
			}
			add_struc_member(pStruct, fieldName.c_str(), 4 * n, dword_flag(), NULL, 4);
		}
		tinfo_t info = create_typedef(vTableName.c_str());
		apply_tinfo(*it,info, TINFO_GUESSED);
	}

	return true;
}



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

void ESymbol::setGuiEventName()
{
	for (std::map<eSymbol_ControlIndex, EAppControl*>::iterator it = allControlMap.begin(); it != allControlMap.end(); ++it) {
		EAppControl* tmpControl = it->second;
		if (!tmpControl) {
			continue;
		}
		for (unsigned int n = 0; n < tmpControl->eventList.size(); ++n) {
			auto eventName = tmpControl->GetEventName(tmpControl->eventList[n].eventIndex);
			qstring funcName;
			funcName.sprnt("_%s_%s", tmpControl->controlName.c_str(), eventName.c_str());
			IDAWrapper::setFuncName(tmpControl->eventList[n].eventAddr, funcName.c_str());
		}
	}
}

void ESymbol::clearControlData()
{
	allControlMap.clear();
	for (unsigned int nControlIndex = 0; nControlIndex < allControlList.size(); ++nControlIndex) {
		EAppControl* eAppControl = allControlList[nControlIndex];
		if (eAppControl) {
			delete eAppControl;
		}
	}
	allControlList.clear();
}
