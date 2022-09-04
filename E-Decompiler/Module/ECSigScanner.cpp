#include "ECSigScanner.h"
#include <string>
#include <map>
#include "../Utils/Common.h"
#include "../Utils/IDAWrapper.h"
#include <pro.h>
#include <fpro.h>
#include <diskio.hpp>

ECSigScanner gECSigScanner;

void initMSig(std::multimap<std::string, std::string>& sigMap)
{
	std::string sigDir = IDAWrapper::idadir("plugins");
	sigDir.append("\\esig");

	std::vector<std::string> sigFileList = IDAWrapper::enumerate_files(sigDir.c_str(), "*.msig");
	for (unsigned int n = 0; n < sigFileList.size(); ++n) {
		FILE* hFile = fopenRB(sigFileList[n].c_str());
		if (!hFile){
			continue;;
		}
		qstring str_Line;
		while (-1 != qgetline(&str_Line, hFile)) {
			size_t spitIndex = str_Line.find(":");
			if (spitIndex == qstring::npos) {
				continue;
			}
			qstring funcName = str_Line.substr(0, spitIndex);
			qstring funcSig = str_Line.substr(spitIndex + 1);
			if (funcSig.last() == '\r') {
				funcSig.remove_last();
			}
			if (funcSig.length() != 32) {
				continue;
			}
			sigMap.insert(std::make_pair(funcSig.c_str(), funcName.c_str()));
		}
		qfclose(hFile);
	}
}

void ScanECSigFunction()
{
	std::multimap<std::string, std::string> map_MSig;
	initMSig(map_MSig);
	if (!map_MSig.size()) {
		return;
	}


}

ECSigScanner& ECSigScanner::Instance()
{
	return gECSigScanner;
}

bool ECSigScanner::ScanECSigFunction()
{

	return true;
}
