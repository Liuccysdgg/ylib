#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsElement.h"
#include "QsWidget.h"

#include "yutil/ini.h"

class QsIni :public ylib::ini
{
public:
	QsIni();
	~QsIni();
	static void regist(kaguya::State& T);
private:
	std::string read(const std::string& name, const std::string& node);
};

