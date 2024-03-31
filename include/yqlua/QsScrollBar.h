#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qscrollbar.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsElement.h"
#include "QsWidget.h"

class QsScrollBar:public QsElement
{
public:
	QsScrollBar(QScrollBar* bar);
	~QsScrollBar();
	static void regist(kaguya::State& T);
private:
	// 通过 QsElement 继承
	virtual QsWidget* toQ() override { return appQs(new QsWidget(m_bar)); }
private:
	QScrollBar* m_bar = nullptr;
};

