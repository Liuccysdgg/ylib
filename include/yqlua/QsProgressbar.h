#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsElement.h"
#include "QsWidget.h"

#include "yqt/progressbar.h"

class QsProgressbar :public ylib::qt::ProgressBar,public QsElement
{
public:
	QsProgressbar(QsWidget* parent = nullptr);
	~QsProgressbar();
	static void regist(kaguya::State& T);
private:
	// 通过 QsElement 继承
	virtual QsWidget* toQ() override{ return appQs(new QsWidget(this)); }
};

