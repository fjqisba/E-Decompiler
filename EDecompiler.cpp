#include "EDecompiler.h"
#include <bytes.hpp>
#include <segment.hpp>
#include <kernwin.hpp>
#include <afx.h>
#include "EGuiInfoObject.h"
#include "IDAMenu.h"
#include "public.h"
#include <ua.hpp>
#include <allins.hpp>

EDecompilerEngine g_MyDecompiler;

bool IsFloatInstruction(ea_t addr)
{
	insn_t ins;
	decode_insn(&ins, addr);
	int a = 0;
	return true;
}

struct chooser_UserResource :public chooser_t 
{
protected:
	const int widths_[3] = { 10,10,50 };
	const char* header_[3] = { "Address","Type","Data" };
public:
	chooser_UserResource(const char* title) :chooser_t(0, qnumber(widths_), widths_, header_, title)
	{

	}
private:
	~chooser_UserResource()
	{

	}
	const void* get_obj_id(size_t* len) const
	{
		*len = strlen(title);
		return title;
	}

	void idaapi get_row(qstrvec_t* cols_, int* icon_, chooser_item_attrs_t* attrs, size_t n) const
	{
		qstrvec_t& cols = *cols_;
		qvector<BinSource>& vec_Bin = g_MyDecompiler.m_eAppInfo.mVec_UserResource;

		cols[0].sprnt("%08a", vec_Bin[n].binAddr);

		if (vec_Bin[n].binData.size())
		{
			if (vec_Bin[n].itype == 0) {
				acp_utf8(&cols[1], "文本型");
				acp_utf8(&cols[2], (char*)&vec_Bin[n].binData[0]);
			}
			else {
				acp_utf8(&cols[1], "字节集");
				cols[2] = 字节集_字节集到十六进制(vec_Bin[n].binData);
			}
		}
		else if (vec_Bin[n].itype == 0) {
			acp_utf8(&cols[1], "空白文本");
		}
		else if (vec_Bin[n].itype == 1) {
			acp_utf8(&cols[1], "空白字节集");
		}
		else if (vec_Bin[n].itype == 2) {
			acp_utf8(&cols[1], "浮点数");
			cols[2].sprnt("%lf", vec_Bin[n].floatData);
		}
	}

	cbret_t idaapi enter(size_t n)
	{
		jumpto(g_MyDecompiler.m_eAppInfo.mVec_UserResource[n].binAddr);
		return cbret_t();
	}
	size_t idaapi get_count(void) const
	{
		return g_MyDecompiler.m_eAppInfo.mVec_UserResource.size();
	}
};

int MenuHandle_ShowUserResource()
{
	qstring 标题;
	acp_utf8(&标题, "用户常量资源");
	chooser_UserResource* pUserChoose = new chooser_UserResource(标题.c_str());
	pUserChoose->choose();
	return 0;
}

bool EDecompilerEngine::ParseGUIResource(ea_t lpGUIStart,uint32 infoSize)
{
	qvector<unsigned char> tmpGuiBuf;
	tmpGuiBuf.resize(infoSize);
	get_bytes(&tmpGuiBuf[0], infoSize, lpGUIStart);

	CMemFile memGuiBuf;
	memGuiBuf.Attach(&tmpGuiBuf[0],infoSize);
	
	CArchive tmpArchive(&memGuiBuf, CArchive::load);
	int windowCount;
	
	EGuiInfoObject GuiObject;
	GuiObject.Serialize(tmpArchive);
	tmpArchive.Close();
	

	int a = 0;
	
	return true;
}

bool EDecompilerEngine::IsArrayHeadValue(ea_t addr)
{
	qvector<ea_t> DataRef = GetAllDataRef(addr);
	if (!DataRef.size()) {
		return false;
	}

	if (DataRef[0] <= m_eAppInfo.m_UserCodeStartAddr || DataRef[0] >= m_eAppInfo.m_UserCodeEndAddr) {
		return false;
	}
	insn_t ins;
	decode_insn(&ins, DataRef[0]);
	if (ins.itype == NN_mov && get_dword(DataRef[0] + 5) == 0xABADABAD) {
		return true;
	}
	return false;
}

bool EDecompilerEngine::IsFloatConstValue(ea_t addr)
{
	qvector<ea_t> DataRef = GetAllDataRef(addr);
	if (!DataRef.size()) {
		return false;
	}

	if (DataRef[0] <= m_eAppInfo.m_UserCodeStartAddr || DataRef[0] >= m_eAppInfo.m_UserCodeEndAddr) {
		return false;
	}
	insn_t ins;
	decode_insn(&ins, DataRef[0]);
	if (ins.itype == NN_fadd || ins.itype == NN_fsub || ins.itype == NN_fmul || ins.itype == NN_fdiv || ins.itype == NN_fcomp || ins.itype == NN_fld)
	{
		return true;
	}
	return false;
}

bool EDecompilerEngine::ParseStringResource(ea_t lpStringStart,uint32 StringSize)
{
	m_eAppInfo.mVec_UserResource.clear();

	qvector<unsigned char> tmpResouceBuf;
	tmpResouceBuf.resize(StringSize);
	get_bytes(&tmpResouceBuf[0], StringSize, lpStringStart);

	for (unsigned int index = 0; index < tmpResouceBuf.size(); ++index) {
		BinSource tmpSource = {};

		if (IsArrayHeadValue(lpStringStart + index)) {         //数组头
			tmpSource.itype = -1;
			index = index + 8 - 1;
		}
		else if (IsFloatConstValue(lpStringStart + index)) {   //检查浮点数或者小数
			tmpSource.itype = 2;
			tmpSource.binAddr = lpStringStart + index;
			get_bytes(&tmpSource.floatData, sizeof(double), tmpSource.binAddr);
			m_eAppInfo.mVec_UserResource.push_back(tmpSource);
			index = index + 8 - 1;
		}
		else if (tmpResouceBuf[index] == 0x0) {
			tmpSource.itype = 0;
			tmpSource.binAddr = lpStringStart + index;
			m_eAppInfo.mVec_UserResource.push_back(tmpSource);
		}
		else if (tmpResouceBuf[index] == 0x1) {
			if ((index + 8) > tmpResouceBuf.size()) {
				return false;
			}
			uint32 binHead = *(uint32*)&tmpResouceBuf[index];
			if (binHead != 0x1) {
				return false;
			}
			uint32 binSize = *(uint32*)&tmpResouceBuf[index + 4];
			if (index + 8 + binSize > tmpResouceBuf.size()) {
				return false;
			}
			tmpSource.itype = 1;
			tmpSource.binAddr = lpStringStart + index;
			if (binSize) {
				tmpSource.binData.resize(binSize);
				memcpy(&tmpSource.binData[0], &tmpResouceBuf[index + 8], binSize);
			}
			m_eAppInfo.mVec_UserResource.push_back(tmpSource);
			index = index + 8 + binSize - 1;
		}
		else {
			tmpSource.itype = 0;
			uint32 sLen = qstrlen(&tmpResouceBuf[index]);
			tmpSource.binAddr = lpStringStart + index;
			tmpSource.binData.resize(sLen + 1);
			memcpy(&tmpSource.binData[0], &tmpResouceBuf[index], sLen + 1);
			m_eAppInfo.mVec_UserResource.push_back(tmpSource);
			index = index + sLen;
		}
	}

	return true;
}

bool EDecompilerEngine::ParseLibInfomation(ea_t lpLibStartAddr, uint32 dwLibCount)
{
	for (unsigned int nLibIndex = 0; nLibIndex < dwLibCount; ++nLibIndex) {
		LIB_INFO tmpLibInfo;
		get_bytes(&tmpLibInfo, sizeof(LIB_INFO), get_dword(lpLibStartAddr));
		lpLibStartAddr = lpLibStartAddr + 4;

		//判断是否符合支持库格式
		if (tmpLibInfo.m_dwLibFormatVer != 0x1312D65) {
			continue;
		}

		mid_ELibInfo eLibInfo;
		eLibInfo.m_Name = get_shortstring(tmpLibInfo.m_lpName);
		eLibInfo.m_Guid = get_shortstring(tmpLibInfo.m_lpGuid);
		eLibInfo.m_nMajorVersion = tmpLibInfo.m_nMajorVersion;
		eLibInfo.m_nMinorVersion = tmpLibInfo.m_nMinorVersion;

		ea_t lpFirstDataType = tmpLibInfo.m_lpDataType;
		for (int nDataTypeIndex = 0; nDataTypeIndex < tmpLibInfo.m_nDataTypeCount; ++nDataTypeIndex) {
			LIB_DATA_TYPE_INFO tmpDataTypeInfo;
			get_bytes(&tmpDataTypeInfo, sizeof(LIB_DATA_TYPE_INFO), lpFirstDataType);
			lpFirstDataType += sizeof(LIB_DATA_TYPE_INFO);

			//验证数据是否真的被抹除了......
			if (tmpDataTypeInfo.m_nCmdCount || tmpDataTypeInfo.m_nEventCount) {
				msg("[E-Decompiler]:Find DataType Field,Please contact the author!\n");
			}

			if (tmpDataTypeInfo.m_lpszName) {
				mid_EDataTypeInfo eDataType;
				eDataType.m_Name = get_shortstring(tmpDataTypeInfo.m_lpszName);
				int a = 0;
			}
		}
	}
	return true;
}

EDecompilerEngine::EDecompilerEngine()
{
	m_ProgramType = E_UNKNOWN;
}

EDecompilerEngine::~EDecompilerEngine()
{
	if (gMenu_ShowResource) {
		gMenu_ShowResource->DestroyMenu();
	}
}

bool EDecompilerEngine::InitDecompilerEngine()
{
	if (!InitSectionManager()) {
		return false;
	}

	m_ProgramType = E_UNKNOWN;

	//探测易语言程序类型
	compiled_binpat_vec_t binPat;
	parse_binpat_str(&binPat, 0x0, "50 64 89 25 00 00 00 00 81 EC AC 01 00 00 53 56 57", 16);
	unsigned int segCount = get_segm_qty();
	ea_t searchAddr = BADADDR;
	for (unsigned int n = 0; n < segCount; ++n) {
		segment_t* pSegment = getnseg(n);
		searchAddr = bin_search2(pSegment->start_ea, pSegment->end_ea, binPat, 0x0);
		if (searchAddr != BADADDR) {
			break;
		}
	}

	if (searchAddr != BADADDR) {
		m_ProgramType = E_STATIC;
		m_EHeadAddr = get_dword(searchAddr + 0x26);
	}

	return true;
}

bool EDecompilerEngine::DoDecompile()
{
	//静态编译程序
	if (m_ProgramType == E_STATIC) {
		return DoDecompiler_EStatic();
	}

	return true;
}

bool EDecompilerEngine::DoDecompiler_EStatic()
{
	EHead eHead;
	get_bytes(&eHead, sizeof(EHead), m_EHeadAddr);
	if (eHead.dwMagic != 0x3) {
		return false;
	}

	m_eAppInfo.m_UserCodeStartAddr = eHead.lpStartCode;
	m_eAppInfo.m_UserCodeEndAddr = m_EHeadAddr;

	if (!eHead.dwLibNum || !eHead.lpLibEntry) {
		return false;
	}
	ParseLibInfomation(eHead.lpLibEntry, eHead.dwLibNum);

	if (eHead.lpEString != 0 && eHead.dwEStringSize != 0) {
		ParseStringResource(eHead.lpEString, eHead.dwEStringSize);
		qstring 菜单_用户资源菜单;
		acp_utf8(&菜单_用户资源菜单, "易语言/用户常量资源");
		gMenu_ShowResource = IDAMenu::CreateMenu(菜单_用户资源菜单.c_str(), MenuHandle_ShowUserResource);
	}

	if (eHead.lpEWindow != 0 && eHead.dwEWindowSize != 0) {
		ParseGUIResource(eHead.lpEWindow, eHead.dwEWindowSize);
	}

	return true;
}