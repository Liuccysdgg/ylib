#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsBuffer.h"
class QsApp
{
public:
	QsApp();
	~QsApp();
	static void regist(kaguya::State& T);
private:
	static void quit();
	static QsBuffer read_res(const std::string& filepath);
};

