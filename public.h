#pragma once
#include <pro.h>

//十六到十
void HexToBin(qstring& HexCode, uchar* BinCode);

//参数一为原文本,参数二为左边文本,参数三为右边文本,参数四为起始偏移
qstring GetMidString(qstring& src, const char* left, const char* right, int offset);

//低配版二进制搜索,用0x90来代替模糊搜索,返回偏移大小
uint32 Search_Bin(unsigned char* pSrc, unsigned char* pTrait, int nSrcLen, int nTraitLen);
