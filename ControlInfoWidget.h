#pragma once
#include <QtWidgets/QtWidgets>
#include "ui_ControlInfoWidget.h"

class ControlInfoWidget:public QWidget
{
public:
	ControlInfoWidget();
	~ControlInfoWidget();
public:
	Ui::ControlInfo ui;
};