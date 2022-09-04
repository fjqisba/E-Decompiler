#pragma once
#include <string>
#include <map>


class ECSigScanner
{
public:
	static ECSigScanner& Instance();
public:
	bool ScanECSigFunction();
private:
	std::map<unsigned int, std::string> savedSubFunc;
};