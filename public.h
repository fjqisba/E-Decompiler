#pragma once
#include <pro.h>

//十六到十
void HexToBin(qstring& HexCode, uchar* BinCode);

//参数一为原文本,参数二为左边文本,参数三为右边文本,参数四为起始偏移
qstring GetMidString(qstring& src, const char* left, const char* right, int offset);

//低配版二进制搜索,用0x90来代替模糊搜索,返回偏移大小
uint32 Search_Bin(unsigned char* pSrc, unsigned char* pTrait, int nSrcLen, int nTraitLen);

qstring 字节集_字节集到十六进制(qvector<unsigned char>& 原始字节集);

//获取所有的数据交叉引用
qvector<ea_t> GetAllDataRef(ea_t addr);

qstring get_shortstring(int32 addr);

qstring getUTF8String(const char*);

uint32 ReadUInt(unsigned char* pBuf);

int32 ReadInt(unsigned char* pBuf);

qstring ReadStr(unsigned char* pBuf);

