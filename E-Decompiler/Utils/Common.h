#pragma once
#include <string>
#include <vector>

unsigned int ReadUInt(unsigned char* pBuf);
int ReadInt(unsigned char* pBuf);
unsigned char ReadUChar(unsigned char* pBuf);
unsigned char ReadUChar(char* pBuf);
std::string ReadStr(unsigned char* pBuf);


//十六到十,a -> 10
unsigned char HexToBin(unsigned char HexCode);

//枚举目录文件

std::vector<std::string> enumDirectoryFiles(const char *dir);

const char* UCharToStr(unsigned char c);