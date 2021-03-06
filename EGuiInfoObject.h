#pragma once
#include <afx.h>
#include <pro.h>

struct eWindowControlInfo
{
	unsigned int m_FieldA;
	unsigned int m_FieldB;
	qvector<unsigned char> m_FiledC;
	qvector<unsigned char> m_FiledD;
	unsigned int m_ControlCount;
	qvector<unsigned char> m_ControlData;
};

class EGuiInfoObject
{
public:
	EGuiInfoObject();
	~EGuiInfoObject();
	void Serialize(CArchive& ar);
public:

private:
	qvector<unsigned char> mVec_WindowsInfo;
};