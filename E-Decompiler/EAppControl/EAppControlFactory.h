#pragma once
#include <map>
#include <string>

typedef void* (*fCreateEAppControl)(void);
struct EAppControl;

class EAppControlFactory
{
public:
	static EAppControlFactory& Instance();
	void RegisterEAppControl(std::string eControlLibInfo,unsigned int controlTypeId);
	EAppControl* CreateEAppControl(unsigned int dwControlTypeId);
private:
	std::map<unsigned int, fCreateEAppControl> registerMap;
};