#pragma once
#include <QtWidgets/QtWidgets>
#include "ui_ControlInfoWidget.h"

class mid_ControlInfo;
class ControlInfoWidget:public QWidget
{
public:
	ControlInfoWidget();
	~ControlInfoWidget();
private:
	void on_controlClicked(QTreeWidgetItem* item, int column);
public:
	Ui::ControlInfo ui;
private:
	void InitKernelControl_Window(mid_ControlInfo*);
	void InitKernelControl_Label(mid_ControlInfo*);
};