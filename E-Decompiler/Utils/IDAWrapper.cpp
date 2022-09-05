#include "IDAWrapper.h"
#include <pro.h>
#include <kernwin.hpp>
#include <bytes.hpp>
#include <diskio.hpp>
#include <name.hpp>
#include <typeinf.hpp>
#include <frame.hpp>

void IDAWrapper::show_wait_box(const char* msg)
{
	::show_wait_box("%s", msg);
}

void IDAWrapper::hide_wait_box()
{
	::hide_wait_box();
}

int IDAWrapper::get_bytes(void* buf, unsigned int size, unsigned int ea, int gmb_flags /*= 0*/, void* mask /*= nullptr*/)
{
	return ::get_bytes(buf, size, ea, gmb_flags, mask);
}

unsigned int IDAWrapper::get_dword(unsigned int ea)
{
	return ::get_dword(ea);
}

unsigned short IDAWrapper::get_word(unsigned int ea)
{
	return ::get_word(ea);
}

std::string IDAWrapper::get_shortstring(unsigned int addr)
{
	if (addr <= 0)
	{
		return "";
	}
	char buffer[255] = { 0 };
	//没读取到完整的字节应该算是错误了
	if (get_bytes(buffer, sizeof(buffer), addr, GMB_READALL, NULL) != sizeof(buffer))
	{
		return "";
	}
	std::string ret = buffer;
	return ret;
}

const char* IDAWrapper::idadir(const char* subdir)
{
	return ::idadir(subdir);
}

void IDAWrapper::setFuncName(unsigned int addr, const char* funcName, bool bForce /*= true*/)
{
	if (!bForce) {
		qstring oldName = ::get_name(addr);
		if (oldName.find("sub_") == qstring::npos) {
			return;
		}
	}
	qstring newName;
	acp_utf8(&newName, funcName);
	set_name(addr, newName.c_str(), SN_NOWARN | SN_FORCE);
}

void IDAWrapper::msg(const char* format, ...)
{
	va_list va;
	va_start(va, format);
	int nbytes = vmsg(format, va);
	va_end(va);
	return;
}

bool IDAWrapper::apply_cdecl(unsigned int ea, const char* decl, int flags /*= 0*/)
{
	til_t* idati = (til_t*)get_idati();
	if (!idati) {
		return false;
	}
	return ::apply_cdecl(idati, ea, decl, flags);
}

std::vector<unsigned int> IDAWrapper::getAllCodeXrefAddr(unsigned int addr)
{
	std::vector<unsigned int> retXrefList;
	auto XrefAddr = get_first_cref_to(addr);
	while (XrefAddr != BADADDR)
	{
		retXrefList.push_back(XrefAddr);
		XrefAddr = get_next_cref_to(addr, XrefAddr);
	}
	return retXrefList;
}

bool IDAWrapper::add_user_stkpnt(unsigned int ea, int delta)
{
	return ::add_user_stkpnt(ea,delta);
}

std::vector<std::string> IDAWrapper::enumerate_files(const char* dir, const char* fname)
{
	std::vector<std::string> retFileList;
	struct MyFileEnumerator :public file_enumerator_t
	{
	public:
		std::vector<std::string>& fileList;
		MyFileEnumerator(std::vector<std::string>& f) :fileList(f) {};
		int visit_file(const char* file)
		{
			fileList.push_back(file);
			return 0;
		}
	};
	MyFileEnumerator fileEnumFunc(retFileList);
	enumerate_files2(0, 0, dir, fname, fileEnumFunc);
	return retFileList;
}



