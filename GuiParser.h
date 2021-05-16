#pragma once
#include <pro.h>
#include "EAppControl/EAppControl.h"

class ControlInfoWidget;

class GuiParser
{
public:
	struct ControlIndex
	{
		unsigned int nWindowIndex;
		unsigned int nControlIndex;
	};
	struct mid_EventInfo
	{
		int m_nEventIndex;       //事件索引
		uint32 m_EventAddr;      //事件地址
	};

	//每个控件都有的基本属性
	struct mid_EBasicProperty
	{
		int m_left;                              //左边
		int m_top;                               //顶边
		int m_width;                             //宽度
		int m_height;                            //高度
		qvector<unsigned int> mVec_childControl; //子控件
		qstring m_tag;                           //标记
	};
	struct mid_ControlInfo
	{
		uint32 m_controlId;                      //控件自身ID
		qstring m_controlName;                   //控件名称
		uint32 m_controlTypeId;                  //控件所属数据类型的ID
		qstring m_controlTypeName;               //控件类型名称
		ea_t m_propertyAddr;                     //属性地址
		int32 m_propertySize;                    //属性大小
		qvector<mid_EventInfo> mVec_eventInfo;   //事件处理
	};
	struct mid_GuiInfo
	{
		uint32 m_windowId;                           //控件所属窗口ID
		qvector<mid_ControlInfo> mVec_ControlInfo;   //窗口中的控件
	};
public:
	//获取控件事件总个数
	static unsigned int GetEventCount();
	static void InitUserCodeStartAddr(ea_t addr);
	static bool ParseGUIResource(ea_t lpGUIStart, uint32 infoSize);
	static int MenuHandle_ShowGuiInfo();
	static int MenuHandle_ShowEventInfo();

	//根据控件类型ID来获得具体的类型
	static ControlType_t GetControlType(unsigned int controlTypeId);
	//根据控件ID直接获取控件属性
	static bool GetControlInfo(unsigned int controlId, mid_ControlInfo& out_ControlInfo);

	static void ClearControlTypeMap();
	static void AddControlType(unsigned int, ControlType_t);
private:
	static void ParseControlBasciProperty(unsigned char* lpControlInfo, GuiParser::mid_ControlInfo& out_Property);
public:
	static ControlInfoWidget* g_ControlInfoWindow;
};