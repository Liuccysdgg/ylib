#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsElement.h"
#include "QsWidget.h"

#include "yqt/combobox.h" 

class QsComBobox :public ylib::qt::ComboBox,public QsElement
{
public:
	QsComBobox(QsWidget* parent = nullptr);
	~QsComBobox();
	static void regist(kaguya::State& T);
private:
	void addItem(const std::string& text);
	// 通过 QsElement 继承
	virtual QsWidget* toQ() override{ return appQs(new QsWidget(this)); }
};

