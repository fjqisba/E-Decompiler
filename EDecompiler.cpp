#include "EDecompiler.h"
#include <bytes.hpp>
#include <segment.hpp>


bool EDecompilerEngine::ParseStringResource(ea_t lpStringStart,uint32 StringSize)
{
	qvector<unsigned char> tmpResouceBuf;
	tmpResouceBuf.resize(StringSize);
	get_bytes(&tmpResouceBuf[0], StringSize, lpStringStart);

	for (unsigned int index = 0; index < tmpResouceBuf.size(); ++index) {
			if (tmpResouceBuf[index] == 0x0) {
				m_eHeadInfo.m_VecStringResource.push_back("");
			}
			else if(tmpResouceBuf[index] == 0x1) {
				if ((index + 8) >= tmpResouceBuf.size()) {
					return false;
				}
				uint32 binHead = *(uint32*)&tmpResouceBuf[index];
				if (binHead != 0x1) {
					return false;
				}
				uint32 binSize = *(uint32*)&tmpResouceBuf[index + 4];
				if (index + 8 + binSize >= tmpResouceBuf.size()) {
					return false;
				}
				qvector<unsigned char> vec_binData;
				if (binSize) {
					vec_binData.resize(binSize);
					memcpy(&vec_binData[0], &tmpResouceBuf[index + 8], binSize);
				}
				m_eHeadInfo.m_VecBinResource.push_back(vec_binData);
				index = index + 8 + binSize - 1;
			}
			else{
				uint32 sLen = qstrlen(&tmpResouceBuf[index]);
				qstring tmpStr((char*)&tmpResouceBuf[index], sLen);
				m_eHeadInfo.m_VecStringResource.push_back(tmpStr);
				index = index + sLen;
			}
	}

	return true;
}

EDecompilerEngine::EDecompilerEngine()
{
	m_ProgramType = E_UNKNOWN;

	//ÃΩ≤‚“◊”Ô—‘≥Ã–Ú¿‡–Õ
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
}

EDecompilerEngine::~EDecompilerEngine()
{

}

bool EDecompilerEngine::DoDecompiler()
{
	//æ≤Ã¨±‡“Î≥Ã–Ú
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

	m_eHeadInfo.m_UserCodeStartAddr = eHead.lpStartCode;

	if (eHead.lpEString != 0 && eHead.dwEStringSize != 0) {
		ParseStringResource(eHead.lpEString, eHead.dwEStringSize);
	}
	return true;
}