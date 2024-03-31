#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "yqlua/QsWidget.h"
class QsHBoxLayout
{
public:
	QsHBoxLayout();
	~QsHBoxLayout();
	QLayout* layout() { return m_layout; }
	static void regist(kaguya::State& T);
private:
	void addStretch();
	void addWidget(QsWidget* widget);

	

private:
	QHBoxLayout* m_layout = nullptr;
};

