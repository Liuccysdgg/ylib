#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qcheckbox.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsElement.h"
#include "QsWidget.h"
#include "QsIcon.h"
class QsCheckBox :public QCheckBox,public QsElement
{
public:
	QsCheckBox(QsWidget* parent = nullptr);
	~QsCheckBox();
	static void regist(kaguya::State& T);
private:
	void setText(const std::string& title);
	void onStateChanged(kaguya::LuaFunction callback) { m_stateChangedCallback = callback; }
	void setIcon(QsIcon icon);
	void setIconSize(int width, int height);
private:
	// 通过 QsElement 继承
	virtual QsWidget* toQ() override{ return appQs(new QsWidget(this)); }
private:
	kaguya::LuaFunction m_stateChangedCallback;
};

