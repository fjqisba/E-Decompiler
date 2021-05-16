#include "oxFF.h"
#include <funcs.hpp>
#include <hexrays.hpp>

namespace oxFF
{
	efunc_t FFDecompile(func_t* pFunc)
	{
		hexrays_failure_t failed;
		cfunc_t* cfunc = decompile(pFunc, &failed);

		efunc_t ret;
		return ret;
	}
}