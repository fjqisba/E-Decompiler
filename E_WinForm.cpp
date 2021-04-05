#include "E_WinForm.h"
#include "public.h"

void Unserialize_WinFormData(unsigned char* lpControlData, WinFrom_UnitData* out_Data)
{
	out_Data->version = ReadUInt(lpControlData);
	lpControlData += 4;

	if (out_Data->version <= 6) {
		out_Data->field_4 = CDR_ReadUInt(lpControlData);
		out_Data->field_8 = CDR_ReadUInt(lpControlData);
		out_Data->field_C = CDR_ReadUInt(lpControlData);

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

		if (out_Data->version >= 2) {
			out_Data->field_38 = CDR_ReadUInt(lpControlData);
		}
		if (out_Data->version >= 3) {
			out_Data->field_3C = CDR_ReadUInt(lpControlData);
			out_Data->field_40 = CDR_ReadUInt(lpControlData);
		}
		if (out_Data->version >= 4) {
			out_Data->field_44 = CDR_ReadUInt(lpControlData);
		}
		if (out_Data->version >= 5) {
			out_Data->field_48 = CDR_ReadUInt(lpControlData);
		}
		if (out_Data->version == 6) {
			out_Data->unknowStringA = CDR_ReadCString(lpControlData);
		}

		out_Data->unknowDataA = CDR_ReadCFreqMem(lpControlData);
		out_Data->unknowDataB = CDR_ReadCFreqMem(lpControlData);
		out_Data->unknowDataC = CDR_ReadCFreqMem(lpControlData);

		out_Data->БъЬт = CDR_ReadCString(lpControlData);
		CDR_ReadCString(lpControlData);
	}
	return;
}