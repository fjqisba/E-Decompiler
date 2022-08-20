#include "ImportsParser.h"
#include "SectionManager.h"
#include <kernwin.hpp>
#include "common/public.h"

qvector<ImportsParser::ImportsApi> ImportsParser::mVec_ImportsApi;

int ImportsParser::MenuHandle_ShowImportsInfo()
{
	struct chooser_eImports :public chooser_multi_t
	{
	protected:
		const int widths_[3] = { 10,10,30 };
		const char* header_[3] = { "Order","LibName","ApiName" };
	public:
		chooser_eImports(const char* title) :chooser_multi_t(0, qnumber(widths_), widths_, header_, title)
		{

		}
	private:
		~chooser_eImports()
		{

		}
		const void* get_obj_id(size_t* len) const
		{
			*len = strlen(title);
			return title;
		}
		void idaapi get_row(qstrvec_t* cols_, int* icon_, chooser_item_attrs_t* attrs, size_t n) const
		{
			qstrvec_t& cols = *cols_;
			qvector<ImportsApi>& vec_Imports = mVec_ImportsApi;
			cols[0].sprnt("0x%X", n);
			if (vec_Imports[n].LibName.empty()) {
				cols[1] = "NULL";
			}
			else {
				cols[1] = vec_Imports[n].LibName;
			}
			cols[2] = vec_Imports[n].ApiName;
		}
		size_t idaapi get_count(void) const
		{
			return mVec_ImportsApi.size();
		}
	};

	qstring title = getUTF8String("用户导入表");
	chooser_eImports* pUserChoose = new chooser_eImports(title.c_str());
	pUserChoose->choose();
	return 0;
}

bool ImportsParser::ParseUserImports(uint32 dwApiCount, uint32 lpModuleName, uint32 lpApiName)
{
	/*mVec_ImportsApi.clear();

	unsigned char* pszLibnameAddr = SectionManager::LinearAddrToVirtualAddr(lpModuleName);
	unsigned char* pszApinameAddr = SectionManager::LinearAddrToVirtualAddr(lpApiName);

	for (unsigned int n = 0; n < dwApiCount; ++n) {

		char* pszLibname = (char*)SectionManager::LinearAddrToVirtualAddr(ReadUInt(pszLibnameAddr));
		char* pszApiname = (char*)SectionManager::LinearAddrToVirtualAddr(ReadUInt(pszApinameAddr));

		ImportsApi eImportsApi;

		acp_utf8(&eImportsApi.LibName, pszLibname);
		acp_utf8(&eImportsApi.ApiName, pszApiname);

		mVec_ImportsApi.push_back(eImportsApi);
		pszLibnameAddr += 4;
		pszApinameAddr += 4;
	}*/
	return true;
}