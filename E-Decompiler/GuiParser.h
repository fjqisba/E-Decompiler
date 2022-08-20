#pragma once
#include <pro.h>

class ControlInfoWidget;

class GuiParser
{
public:
	struct ControlIndex
	{
		unsigned int nWindowIndex;
		unsigned int nControlIndex;
	};

	//每个控件都有的基本属性
	struct mid_EBasicProperty
	{
		int m_left;                              //左边
		int m_top;                               //顶边
		int m_width;                             //宽度
		int m_height;                            //高度
		qvector<unsigned int> mVec_childControl; //子控件
		std::string m_tag;                       //标记
	};

public:
	//获取控件事件总个数
	static unsigned int GetEventCount();
	static void InitUserCodeStartAddr(ea_t addr);
	
	static int MenuHandle_ShowGuiInfo();
	static int MenuHandle_ShowEventInfo();

	//根据控件类型ID来获得具体的类型
	//static ControlType_t GetControlType(unsigned int controlTypeId);
	//根据控件ID直接获取控件属性
	//static bool GetControlInfo(unsigned int controlId, mid_ControlInfo& out_ControlInfo);

	//static void AddControlType(unsigned int, ControlType_t);

public:
	static ControlInfoWidget* g_ControlInfoWindow;
};