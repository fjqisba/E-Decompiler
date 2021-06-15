#include "SectionManager.h"
#include <segment.hpp>
#include <bytes.hpp>

qvector<SegmentInfomation> SectionManager::mVec_segInfo;
qvector<unsigned char> SectionManager::m_AllMemBuf;

bool SectionManager::InitSectionManager()
{
	mVec_segInfo.clear();
	m_AllMemBuf.clear();

	int segCount = get_segm_qty();
	
	unsigned int bufSize = 0;
	for (int idx = 0; idx < segCount; ++idx)
	{
		SegmentInfomation tmpInfo;
		segment_t* pSegment = getnseg(idx);
		tmpInfo.m_segStart = pSegment->start_ea;
		tmpInfo.m_segSize = pSegment->size();

		bufSize += tmpInfo.m_segSize;
		qstring SectionName;
		get_segm_name(&SectionName, pSegment);
		tmpInfo.m_segName = SectionName;

		tmpInfo.m_segData.resize(pSegment->size());
		get_bytes(&tmpInfo.m_segData[0], pSegment->size(), pSegment->start_ea, GMB_READALL);
		mVec_segInfo.push_back(tmpInfo);
	}

	m_AllMemBuf.resize(bufSize);
	get_bytes(&m_AllMemBuf[0], bufSize, mVec_segInfo[0].m_segStart, GMB_READALL);
	return true;
}

ea_t SectionManager::SeachBin(qstring HexStr)
{
	ea_t ret = BADADDR;
	
	compiled_binpat_vec_t binPat;
	parse_binpat_str(&binPat, 0x0, HexStr.c_str(), 16);

	unsigned int segCount = get_segm_qty();
	for (unsigned int n = 0; n < segCount; ++n) {
		segment_t* pSegment = getnseg(n);
		if (!pSegment) {
			continue;
		}
		ret = bin_search2(pSegment->start_ea, pSegment->end_ea, binPat, 0x0);
		if (ret != BADADDR) {
			break;
		}
	}

	return ret;
}

uint8* SectionManager::LinearAddrToVirtualAddr(ea_t LinerAddr)
{
	int offset = LinerAddr - mVec_segInfo[0].m_segStart;
	if (offset < 0) {
		return NULL;
	}
	return &m_AllMemBuf[offset];
}


ea_t SectionManager::VirtualAddrToLinearAddr(uint8* pVirtualAddr)
{
	int offset= pVirtualAddr- &m_AllMemBuf[0];
	if (offset < 0) {
		return BADADDR;
	}
	return mVec_segInfo[0].m_segStart + offset;
}