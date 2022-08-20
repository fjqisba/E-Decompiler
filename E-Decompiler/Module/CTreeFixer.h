#pragma once

//修复易语言反编译结果

class ESymbol;
class CTreeFixer
{
public:
	CTreeFixer(ESymbol& symbol);
	~CTreeFixer();
private:
	ESymbol& eSymbol;
};