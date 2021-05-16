#pragma once
#include <pro.h>

struct einsn_t;

struct eblock_t:public qlist<einsn_t>
{

};

struct eexpr_t
{

};

struct einsn_t
{
	union
	{
		eblock_t* eblock;   ///< details of block-statement
		eexpr_t* eexpr;     ///< details of expression-statement
	};
};

//用于保存反编译结果
struct efunc_t
{
	ea_t entry_ea;
	einsn_t body;
};

class func_t;
namespace oxFF
{
	efunc_t FFDecompile(func_t*);
}