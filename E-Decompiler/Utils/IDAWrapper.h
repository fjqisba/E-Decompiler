#pragma once
#include <string>

namespace IDAWrapper{
	void show_wait_box(const char* msg);
	void hide_wait_box();
	int get_bytes(void* buf, unsigned int size,unsigned int ea,int gmb_flags = 0,void* mask = nullptr);
	unsigned int get_dword(unsigned int ea);
	std::string get_shortstring(unsigned int addr);

	const char* idadir(const char* subdir);

	//ÉèÖÃº¯ÊıÃû³Æ
	void setFuncName(unsigned int addr, const char* funcName, bool bForce = true);

	void msg(const char* format, ...);

	bool apply_cdecl(unsigned int ea, const char* decl, int flags = 0);
}