#pragma once
#include <pro.h>

class ImportsParser
{
public:
	struct ImportsApi
	{
		qstring LibName;
		qstring ApiName;
	};
public:
	static bool ParseUserImports(uint32 dwApiCount, uint32 lpModuleName,uint32 lpApiName);
	static int MenuHandle_ShowImportsInfo();
public:
	static qvector<ImportsApi> mVec_ImportsApi;
};