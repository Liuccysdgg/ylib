#pragma once
#include "QtCore/qtimer.h"
#include <string>
#include <Kaguya/kaguya.hpp>
#include "yutil/ini.h"

class QsTimer :public QTimer
{
public:
	QsTimer();
	~QsTimer();
	static void regist(kaguya::State& T);
private:
	void onTimeout(kaguya::LuaFunction callback) { m_onTimeoutCallback = callback; };
	void start(int msec);
private:
	kaguya::LuaFunction m_onTimeoutCallback;
};

