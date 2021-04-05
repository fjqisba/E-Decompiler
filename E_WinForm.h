#pragma once
#include <pro.h>

struct WinFrom_UnitData
{
	unsigned int version;
	unsigned int field_4;
	unsigned int field_8;
	unsigned int field_C;
	unsigned int field_10;
	unsigned int field_14;
	unsigned int field_18;
	unsigned int field_1C;
	unsigned int field_20;
	unsigned int field_24;
	unsigned int field_28;
	unsigned int field_2C;
	unsigned int field_30;
	unsigned int field_34;
	unsigned int field_38;
	unsigned int field_3C;
	unsigned int field_40;
	unsigned int field_44;
	unsigned int field_48;
	qstring unknowStringA;
	qvector<unsigned char> unknowDataA;
	qvector<unsigned char> unknowDataB;
	qvector<unsigned char> unknowDataC;
	qstring БъЬт;
};

void Unserialize_WinFormData(unsigned char* lpControlData, WinFrom_UnitData* out_Data);