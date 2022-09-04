#pragma once
#include <string>
#include <vector>

namespace IDAWrapper{
	void show_wait_box(const char* msg);
	void hide_wait_box();
	int get_bytes(void* buf, unsigned int size,unsigned int ea,int gmb_flags = 0,void* mask = nullptr);
	unsigned int get_dword(unsigned int ea);
	unsigned short get_word(unsigned int ea);
	std::string get_shortstring(unsigned int addr);

	const char* idadir(const char* subdir);

	//设置函数名称
	void setFuncName(unsigned int addr, const char* funcName, bool bForce = true);

	void msg(const char* format, ...);

	bool apply_cdecl(unsigned int ea, const char* decl, int flags = 0);

	//获取代码交叉引用地址
	std::vector<unsigned int> getAllCodeXrefAddr(unsigned int addr);

	bool add_user_stkpnt(unsigned int ea, int delta);
	
	//枚举指定目录文件
	std::vector<std::string> enumerate_files(const char* dir, const char* fname);
}