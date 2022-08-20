#pragma once
#include <idp.hpp>

class Plugin :public plugmod_t
{
public:
	Plugin();
	~Plugin();
protected:
	virtual bool idaapi run(size_t) override;
private:

};