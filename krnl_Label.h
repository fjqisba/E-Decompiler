#pragma once
#include <pro.h>
#include "EControl.h"

struct Label_UnitData
{
	unsigned int version;
	unsigned int 效果;
	unsigned int filed_8;
	unsigned int filed_C;
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
	qvector<unsigned char> field_44;
	qvector<unsigned char> field_48;
	qstring field_4C;
	qstring field_50;
	qstring field_54;
};

class krnl_Label
{
public:
	
};

bool Unserialize_LabelData(unsigned char* lpControlData, Label_UnitData* out_Data);

qstring 取标签属性_效果(unsigned int index);