#pragma once
#include <ida.hpp>
#include <windows.h>
#include <funcs.hpp>


class TrieTreeNode
{
public:
	TrieTreeNode();
	qvector<TrieTreeNode*> SpecialNodes;
	TrieTreeNode** ChildNodes;

	uint32 SpecialType;	//一个数字代表类型
	char* EsigText;		//一段文字代表数据
	char* FuncName;		//函数名称
	bool IsMatched;     //是否已经匹配过
};

class TrieTree
{
public:
	TrieTree();
	~TrieTree();

	//日志,打印子函数结果
	void Log_PrintSubFunc();
	//加载特征码
	bool LoadSig(const char* lpMapPath);
	//执行函数匹配
	char* MatchFunc(unsigned char* CodeSrc);
private:
	//增加普通节点
	TrieTreeNode* AddNode(TrieTreeNode* p, qstring Txt);
	//增加特殊节点
	TrieTreeNode* AddSpecialNode(TrieTreeNode* p, uint type, qstring Txt);

	//快速匹配特征码
	bool FastMatch(TrieTreeNode* p, unsigned char*& FuncSrc);
	//慢速匹配特征码
	bool SlowMatch(unsigned char* FuncSrc, qstring& FuncTxt);

	bool SlowMatch_CmpCallApi(unsigned char* pSrc, qstring IATEAT);
	bool SlowMatch_CmpCall(unsigned char* pSrc, qstring FuncName);
public:
	qvector<char*>  MemAllocSave;

	//修改函数的名称
	bool m_IsSetName;
	bool m_IsAligned;
	bool m_IsAllMem;
	bool m_MatchSubName;
protected:
	bool Insert(qstring& FuncTxt, const qstring& FuncName);

private:
	enum NodeType_t
	{
		NODE_NORMAL = 0,
		NODE_LONGJMP = 1,	       //      -->
		NODE_CALL = 2,	           //      <>
		NODE_JMPAPI = 3,	       //      []
		NODE_CALLAPI = 4,	       //      <[]>
		NODE_CONSTANT = 6,	       //      !!
		NODE_LEFTPASS = 11,        //      ?
		NODE_RIGHTPASS = 12,       //       ?
		NODE_ALLPASS = 13          //      ??
	};
	//根节点
	TrieTreeNode* root;
	//func_t* func;

	//子函数,函数名称和函数文本一一映射
	std::map<qstring, qstring> m_subFunc;	

	 //R代表Runtime,运行时记录实际地址对应函数,不要试图一个地址多个函数名称 ,参数一为实际内存地址,参数二为对应名称
	std::map<uint32, qstring> m_RFunc; 
};