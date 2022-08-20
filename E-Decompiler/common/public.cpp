#include "public.h"
#include <xref.hpp>
#include <bytes.hpp>
#include <name.hpp>
#include "md5.h"

const char* UCharToStr(unsigned char c)
{
	static const char* ByteMap[256] = {
		"00","01","02","03","04","05","06","07","08","09","0A","0B","0C","0D","0E","0F",
		"10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F",
		"20","21","22","23","24","25","26","27","28","29","2A","2B","2C","2D","2E","2F",
		"30","31","32","33","34","35","36","37","38","39","3A","3B","3C","3D","3E","3F",
		"40","41","42","43","44","45","46","47","48","49","4A","4B","4C","4D","4E","4F",
		"50","51","52","53","54","55","56","57","58","59","5A","5B","5C","5D","5E","5F",
		"60","61","62","63","64","65","66","67","68","69","6A","6B","6C","6D","6E","6F",
		"70","71","72","73","74","75","76","77","78","79","7A","7B","7C","7D","7E","7F",
		"80","81","82","83","84","85","86","87","88","89","8A","8B","8C","8D","8E","8F",
		"90","91","92","93","94","95","96","97","98","99","9A","9B","9C","9D","9E","9F",
		"A0","A1","A2","A3","A4","A5","A6","A7","A8","A9","AA","AB","AC","AD","AE","AF",
		"B0","B1","B2","B3","B4","B5","B6","B7","B8","B9","BA","BB","BC","BD","BE","BF",
		"C0","C1","C2","C3","C4","C5","C6","C7","C8","C9","CA","CB","CC","CD","CE","CF",
		"D0","D1","D2","D3","D4","D5","D6","D7","D8","D9","DA","DB","DC","DD","DE","DF",
		"E0","E1","E2","E3","E4","E5","E6","E7","E8","E9","EA","EB","EC","ED","EE","EF",
		"F0","F1","F2","F3","F4","F5","F6","F7","F8","F9","FA","FB","FC","FD","FE","FF"
	};
	return ByteMap[c];
}







uint32 Search_Bin(unsigned char* pSrc, unsigned char* pTrait, int nSrcLen, int nTraitLen)
{
	//To do...
	return 0;
}

qstring 字节集_字节集到十六进制(qvector<unsigned char>& 原始字节集)
{
	auto BufLen = 原始字节集.size();
	if (!BufLen)
	{
		return "";
	}
	qstring ret;
	ret.resize(BufLen * 2 + 1, 0x0);
	int Index = 0;
	for (size_t n = 0; n < 原始字节集.size(); n++)
	{
		const char* pHex = UCharToStr(原始字节集[n]);
		ret[Index++] = pHex[0];
		ret[Index++] = pHex[1];
	}
	return ret;
}

qvector<ea_t> GetAllDataRef(ea_t addr)
{
	qvector<ea_t> ret;
	ea_t XrefAddr = get_first_dref_to(addr);
	while (XrefAddr != BADADDR)
	{
		ret.push_back(XrefAddr);
		XrefAddr = get_next_dref_to(addr, XrefAddr);
	}
	return ret;
}

bool HasDataRef(ea_t addr)
{
	if (get_first_dref_to(addr) == BADADDR) {
		return false;
	}
	return true;
}


qstring getUTF8String(const char* data)
{
	qstring out;
	acp_utf8(&out, data);
	return out;
}



int CDR_ReadInt(unsigned char*& currentPoint)
{
	int ret = *(int32*)currentPoint;
	currentPoint += 4;
	return ret;
}

unsigned short CDR_ReadUShort(unsigned char*& currentPoint)
{
	unsigned short ret = *(uint16*)currentPoint;
	currentPoint += 2;
	return ret;
}

char CDR_ReadChar(unsigned char*& currentPoint)
{
	char ret = *(uint8*)currentPoint;
	currentPoint += 2;
	return ret;
}

unsigned int CDR_ReadUInt(unsigned char*& currentPoint)
{
	unsigned int ret = *(uint32*)currentPoint;
	currentPoint += 4;
	return ret;
}

qvector<qstring> CDR_ReadVecString(unsigned char*& currentPoint)
{
	qvector<qstring> ret;
	unsigned int len = CDR_ReadUShort(currentPoint);
	if (len == 0xFFFF) {
		len = CDR_ReadUInt(currentPoint);
	}
	for (unsigned int n = 0; n < len; ++n) {
		ret.push_back(CDR_ReadCString(currentPoint));
	}
	return ret;
}

qvector<unsigned int> CDR_ReadVecInt(unsigned char*& currentPoint)
{
	qvector<unsigned int> ret;
	int len = CDR_ReadInt(currentPoint);
	for (int n = 0; n < len / 4; ++n) {
		ret.push_back(CDR_ReadUInt(currentPoint));
	}
	return ret;
}

qstring CDR_ReadCString(unsigned char*& currentPoint)
{
	qstring ret;
	int len = CDR_ReadInt(currentPoint);
	if (len > 0) {
		ret.resize(len);
		memcpy(&ret[0], currentPoint, len);
		currentPoint += len;
	}
	return ret;
}


qvector<unsigned char> CDR_ReadCFreqMem(unsigned char*& currentPoint)
{
	qvector<unsigned char> ret;
	int len = CDR_ReadInt(currentPoint);
	if (len > 0) {
		ret.resize(len);
		memcpy(&ret[0], currentPoint, len);
		currentPoint += len;
	}
	return ret;
}

qstring CalculateMD5(qstring& md5)
{
	return md5sum(md5.c_str(), md5.length());
}

bool acp_ascii(qstring* out, const char* in)
{
	return change_codepage(out, in, CP_UTF8, CP_ACP);
}

