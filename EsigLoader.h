#pragma once
#include <ida.hpp>

struct outputFunc
{
	ea_t FuncAddr;
	qstring FuncName;
};

struct EsigInfo
{
	qstring Category;
	qstring Name;
	qstring Description;
	qstring Path;
};

//选择特征码文件
qstring ChooseEsig();

//应用特征码文件
bool ApplyEsig(qstring sigPath);