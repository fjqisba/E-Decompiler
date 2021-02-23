#pragma once
#include <ida.hpp>
#include <windows.h>
#include <funcs.hpp>

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

	void Log_PrintSubFunc();
	bool LoadSig(const char* lpMapPath);

	//char* MatchSig(UCHAR*);					//单点匹配
	//
	//增加普通节点
	TrieTreeNode* AddNode(TrieTreeNode* p, qstring Txt);
	//增加特殊节点
	TrieTreeNode* AddSpecialNode(TrieTreeNode* p, uint type, qstring Txt);

public:
	qvector<char*>  MemAllocSave;

	//修改函数的名称
	bool m_IsSetName;
	bool m_IsAligned;
	bool m_IsAllMem;
	bool m_MatchSubName;
protected:
	bool Insert(qstring& FuncTxt, const qstring& FuncName);
	bool CmpCode(uchar* FuncSrc, qstring& FuncTxt);
private:
	//根节点
	TrieTreeNode* root;
	//func_t* func;

	//char* Match(TrieTreeNode* p, UCHAR* FuncSrc);			 //参数一为匹配节点,参数二为匹配地址,返回匹配成功的函数文本
	//BOOL CheckNode(TrieTreeNode* p, UCHAR*& FuncSrc);		//当前特殊节点是否匹配

	//子函数,函数名称和函数文本一一映射
	std::map<qstring, qstring> m_subFunc;	

	 //R代表Runtime,运行时记录实际地址对应函数,不要试图一个地址多个函数名称 ,参数一为实际内存地址,参数二为对应名称
	std::map<uint32, qstring> m_RFunc; 
};