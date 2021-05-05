#include "SectionManager.h"
#include <segment.hpp>
#include <bytes.hpp>

qvector<SegmentInfomation> SectionManager::mVec_segInfo;

bool SectionManager::InitSectionManager()
{
	mVec_segInfo.clear();
	int segCount = get_segm_qty();
	for (int idx = 0; idx < segCount; ++idx)
	{
		SegmentInfomation tmpInfo;
		segment_t* pSegment = getnseg(idx);

		tmpInfo.m_segStart = pSegment->start_ea;
		tmpInfo.m_segSize = pSegment->size();

		qstring SectionName;
		get_segm_name(&SectionName, pSegment);
		tmpInfo.m_segName = SectionName;

		tmpInfo.m_segData.resize(pSegment->size());
		get_bytes(&tmpInfo.m_segData[0], pSegment->size(), pSegment->start_ea, GMB_READALL);

		mVec_segInfo.push_back(tmpInfo);
	}
	return true;
}

uint8* SectionManager::LinearAddrToVirtualAddr(ea_t LinerAddr)
{
	segment_t* pSegment = getseg(LinerAddr);
	if (!pSegment) {
		return NULL;
	}
	
	for (unsigned int n = 0; n < mVec_segInfo.size(); ++n) {
		ea_t endAddr = mVec_segInfo[n].m_segStart + mVec_segInfo[n].m_segSize;
		if (LinerAddr >= mVec_segInfo[n].m_segStart && LinerAddr < endAddr) {
			uint32 offset = LinerAddr - pSegment->start_ea;
			return &mVec_segInfo[n].m_segData[offset];
		}
	}
	return NULL;
}


ea_t SectionManager::VirtualAddrToLinearAddr(uint8* pVirtualAddr)
{
	for (unsigned int n = 0; n < mVec_segInfo.size(); ++n) {
		uint8* pEndAddr = &mVec_segInfo[n].m_segData[0] + mVec_segInfo[n].m_segSize;
		if (pVirtualAddr >= &mVec_segInfo[n].m_segData[0] && pVirtualAddr < pEndAddr) {
			uint32 offset = pVirtualAddr - &mVec_segInfo[n].m_segData[0];
			return mVec_segInfo[n].m_segStart + offset;
		}
	}
	return BADADDR;
}