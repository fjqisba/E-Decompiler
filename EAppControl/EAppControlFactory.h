#pragma once

enum ControlType_t;
class EAppControl;
class EAppControlFactory
{
public:
	static EAppControl* getEAppControl(ControlType_t type);
};