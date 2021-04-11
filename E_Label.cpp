#include "E_Label.h"
#include "public.h"

bool Unserialize_LabelData(unsigned char* lpControlData, Label_UnitData* out_Data)
{
	out_Data->version = CDR_ReadUInt(lpControlData);
	if (out_Data->version > 2) {
		return false;
	}

	out_Data->效果 = CDR_ReadUInt(lpControlData);
	out_Data->filed_8 = CDR_ReadUInt(lpControlData);
	out_Data->filed_C = CDR_ReadUInt(lpControlData);

	out_Data->field_10 = CDR_ReadUInt(lpControlData);
	out_Data->field_14 = CDR_ReadUInt(lpControlData);
	out_Data->field_18 = CDR_ReadUInt(lpControlData);
	out_Data->field_1C = CDR_ReadUInt(lpControlData);
	out_Data->field_20 = CDR_ReadUInt(lpControlData);
	out_Data->field_24 = CDR_ReadUInt(lpControlData);
	out_Data->field_28 = CDR_ReadUInt(lpControlData);
	out_Data->field_2C = CDR_ReadUInt(lpControlData);
	out_Data->field_30 = CDR_ReadUInt(lpControlData);
	out_Data->field_34 = CDR_ReadUInt(lpControlData);
	out_Data->field_38 = CDR_ReadUInt(lpControlData);
	out_Data->field_3C = CDR_ReadUInt(lpControlData);
	out_Data->field_40 = CDR_ReadUInt(lpControlData);

	out_Data->field_44 = CDR_ReadCFreqMem(lpControlData);
	out_Data->field_48 = CDR_ReadCFreqMem(lpControlData);

	out_Data->field_4C = CDR_ReadCString(lpControlData);
	if (out_Data->version == 2) {
		out_Data->field_50 = CDR_ReadCString(lpControlData);
		out_Data->field_54 = CDR_ReadCString(lpControlData);
	}

	return true;
}

qstring 取标签属性_效果(unsigned int index)
{
	qstring ret;
	switch (index)
	{
	case 0:
		ret = "通常";
		break;
	case 1:
		ret = "凹入";
		break;
	case 2:
		ret = "凸出";
		break;
	case 3:
		ret = "阴影";
		break;
	default:
		ret = "未知效果";
		break;
	}
	return ret;
}