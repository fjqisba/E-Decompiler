#pragma once
#include <pro.h>

struct SegmentInfomation
{
	ea_t m_segStart;                   //区段起始地址
	uint32 m_segSize;                  //区段大小
	qstring m_segName;                 //区段名称
	qvector<unsigned char> m_segData;  //区段数据
};

class SectionManager
{
public:
	SectionManager();
	~SectionManager();
public:
	//初始化程序区段
	bool InitSectionManager();
	//线性地址转换为虚拟地址
	uint8* LinearAddrToVirtualAddr(ea_t LinerAddr);
	//虚拟地址转换为线性地址
	uint32 VirtualAddrToLinearAddr(ea_t dwOaddr);
private:
	qvector<SegmentInfomation> mVec_segInfo;
};





