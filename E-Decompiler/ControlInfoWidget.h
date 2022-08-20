#pragma once
#include <QtWidgets/QtWidgets>
#include "ui_ControlInfoWidget.h"


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
	
};