#include "EGuiInfoObject.h"
#include <afx.h>


EGuiInfoObject::EGuiInfoObject()
{

}

EGuiInfoObject::~EGuiInfoObject()
{

}

qvector<unsigned char> ReadBytes(CArchive& ar)
{
	qvector<unsigned char> ret;
	int ReadSize;
	ar >> ReadSize;
	if (ReadSize > 0)
	{
		ret.resize(ReadSize);
		ar.Read(&ret[0], ReadSize);
	}
	return ret;
}

eWindowControlInfo ReadControlInfo(CArchive& ar)
{
	eWindowControlInfo ret;
	ar >> ret.m_FieldA >> ret.m_FieldB;

	ret.m_FiledC = ReadBytes(ar);
	ret.m_FiledD = ReadBytes(ar);

	ar >> ret.m_ControlCount;

	ret.m_ControlData = ReadBytes(ar);
	return ret;
}

void EGuiInfoObject::Serialize(CArchive& ar)
{
	mVec_WindowsInfo = ReadBytes(ar);
	for (unsigned int n = 0; n < mVec_WindowsInfo.size() >> 3; ++n)
	{
		eWindowControlInfo tmpInfo = ReadControlInfo(ar);

		for (unsigned int m = 0; m < tmpInfo.m_ControlCount; ++m)
		{
			qvector<unsigned int> mVec_ControlId;

			int a = 0;
		}
		int d = 0;
	}



	return;
}
