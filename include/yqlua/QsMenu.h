#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsElement.h"
#include "QsWidget.h"
#include <QtWidgets/qmenu.h>
#include "QsIcon.h"
class QsAction
{
public:
	QsAction();
	~QsAction();
	static void regist(kaguya::State& T);
	QAction* action() { return m_action; }

	void onTriggered(kaguya::LuaFunction callback) { m_onTriggered = callback; };
private:
	void setText(const std::string& text);
	void setIcon(QsIcon icon);
private:
	QAction* m_action = nullptr;
	kaguya::LuaFunction m_onTriggered;
};
class QsMenu :public QsElement
{
public:
	QsMenu(QsWidget* parent = nullptr);
	~QsMenu();
	static void regist(kaguya::State& T);
private:
	void addAction(QsAction* action);
	void exec();
	// 通过 QsElement 继承
	virtual QsWidget* toQ() override{ return appQs(new QsWidget(m_menu)); }
private:
	QMenu* m_menu = nullptr;
};

