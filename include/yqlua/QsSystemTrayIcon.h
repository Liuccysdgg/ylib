#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsElement.h"
#include "QsWidget.h"
#include "QsIcon.h"
#include <QtWidgets/qsystemtrayicon.h>
class QsSystemTrayIcon 
{
public:
	QsSystemTrayIcon(QsWidget* widget);
	~QsSystemTrayIcon();
	static void regist(kaguya::State& T);
private:
	void setIcon(const QsIcon& icon);
	void setToolTip(const std::string& msg);
	void onActivated(kaguya::LuaFunction callback) { m_onActivated = callback; }
	void hide();
	void show();
private:
	kaguya::LuaFunction m_onActivated;
	QSystemTrayIcon* m_tray = nullptr;
};

