#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qheaderview.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsElement.h"
#include "QsWidget.h"

class QsHeaderView:public QsElement
{
public:
	QsHeaderView(QHeaderView* view);
	~QsHeaderView();
	static void regist(kaguya::State& T);
private:
	void setSectionResizeMode(int logicalIndex,int mode);
	// 通过 QsElement 继承
	virtual QsWidget* toQ() override { return appQs(new QsWidget(m_view)); }
private:
	QHeaderView* m_view = nullptr;
};

