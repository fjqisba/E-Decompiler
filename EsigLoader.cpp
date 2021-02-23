#include "EsigLoader.h"
#include <kernwin.hpp>
#include <diskio.hpp>
#include <fpro.h>
#include <pro.h>
#include "public.h"
#include "TrieTree.h"

qvector<EsigInfo> g_EsigList;
qvector<outputFunc> g_SaveFunc;

struct Esig_chooser :public chooser_t
{
protected:
	static const int widths[];
	static const char* const headers[];

public:
	Esig_chooser();

	//以下需要自己重写类的虚函数
	virtual const void* get_obj_id(size_t* len) const
	{
		*len = strlen(title);
		return title;
	}

	virtual size_t idaapi get_count() const {
		return g_EsigList.size();
	}

	virtual void idaapi get_row(
		qstrvec_t* cols_,
		int*,
		chooser_item_attrs_t*,
		size_t n)const
	{
		qstrvec_t& cols = *cols_;
		acp_utf8(&cols[0], g_EsigList[n].Name.c_str());
		acp_utf8(&cols[1], g_EsigList[n].Description.c_str());
	}
};

const int Esig_chooser::widths[] =
{
	23,
	38,
};

const char* const Esig_chooser::headers[] =
{
	"FileName",
	"Description",
};

Esig_chooser::Esig_chooser() : chooser_t(CH_MODAL, qnumber(widths), widths, headers, "List of available Easy Signature")
{
	x0 = 0;
	y0 = 0;
	x1 = 200;
	y1 = 200;
}

int idaapi EnumEsig(const char* lpMapPath, void* ud)
{
	FILE* hFile = qfopen(lpMapPath, "rb");
	if (!hFile) {
		return 0;
	}

	uint32 dwSize= qfsize(hFile);
	char* pMap = (char*)malloc(dwSize);
	qfread(hFile, pMap, dwSize);

	qstring Sig = pMap;
	qstring Config = GetMidString(Sig, "******Config******\r\n", "******Config_End******", 0);
	qstring Name = GetMidString(Sig, "Name=", "\r\n", 0);
	qstring Description = GetMidString(Sig, "Description=", "\r\n", 0);

	if (!Name.size()) {
		return 0;
	}

	EsigInfo saveinfo;
	saveinfo.Name = Name;
	saveinfo.Description = Description;
	saveinfo.Path = lpMapPath;

	g_EsigList.push_back(saveinfo);

	if (pMap) {
		free(pMap);
	}

	qfclose(hFile);
	return 0;
}

qstring ChooseEsig()
{
	g_EsigList.clear();
	//先枚举全部的Esig文件
	qstring esigPath;
	esigPath.sprnt("%s\\esig", idadir("plugins"));

	char buffer[MAXSTR] = { 0 };
	enumerate_files(buffer, MAXSTR, esigPath.c_str(), "*.esig", EnumEsig, NULL);

	qstring retPath;
	Esig_chooser* EsigWindow = new Esig_chooser();
	int index = EsigWindow->choose();
	if (index == -1) {
		return retPath;
	}
	return g_EsigList[index].Path;
}

bool ApplyEsig(qstring sigPath)
{
	g_SaveFunc.clear();
	TrieTree Tree;
	Tree.LoadSig(sigPath.c_str());

	return true;
}