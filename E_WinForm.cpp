#include "E_WinForm.h"
#include "public.h"

void Unserialize_WinFormData(unsigned char* lpControlData, WinFrom_UnitData* out_Data)
{
	out_Data->version = ReadUInt(lpControlData);
	lpControlData += 4;

	if (out_Data->version <= 6) {
		out_Data->边框 = CDR_ReadUInt(lpControlData);
		out_Data->底图方式 = CDR_ReadUInt(lpControlData);
		out_Data->底色 = CDR_ReadUInt(lpControlData);

		out_Data->最大化按钮 = CDR_ReadUInt(lpControlData);
		out_Data->最小化按钮 = CDR_ReadUInt(lpControlData);
		out_Data->控制按钮 = CDR_ReadUInt(lpControlData);
		out_Data->位置 = CDR_ReadUInt(lpControlData);
		out_Data->可否移动 = CDR_ReadUInt(lpControlData);
		out_Data->播放次数 = CDR_ReadUInt(lpControlData);
		out_Data->回车下移焦点 = CDR_ReadUInt(lpControlData);
		out_Data->Esc键关闭 = CDR_ReadUInt(lpControlData);
		out_Data->F1键打开帮助 = CDR_ReadUInt(lpControlData);
		out_Data->field_34 = CDR_ReadUInt(lpControlData);

		if (out_Data->version >= 2) {
			out_Data->field_38 = CDR_ReadUInt(lpControlData);
		}
		if (out_Data->version >= 3) {
			out_Data->field_3C = CDR_ReadUInt(lpControlData);
			out_Data->field_40 = CDR_ReadUInt(lpControlData);
		}
		if (out_Data->version >= 4) {
			out_Data->总在最前 = CDR_ReadUInt(lpControlData);
		}
		if (out_Data->version >= 5) {
			out_Data->保持标题条激活 = CDR_ReadUInt(lpControlData);
		}
		if (out_Data->version == 6) {
			out_Data->窗口类名 = CDR_ReadCString(lpControlData);
		}

		out_Data->底图 = CDR_ReadCFreqMem(lpControlData);
		out_Data->图标 = CDR_ReadCFreqMem(lpControlData);
		out_Data->背景音乐 = CDR_ReadCFreqMem(lpControlData);

		out_Data->标题 = CDR_ReadCString(lpControlData);
		out_Data->帮助文件名 = CDR_ReadCString(lpControlData);
	}
	return;
}