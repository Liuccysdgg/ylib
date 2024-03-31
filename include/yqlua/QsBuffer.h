#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "ybase/buffer.h"
class QsBuffer :public ylib::buffer
{
public:
	QsBuffer();
	~QsBuffer();
	static void regist(kaguya::State& T);


public:
	static QsBuffer fromString(const std::string& value);
	void append(unsigned long long data_point,unsigned int length);
	bool saveToFile(const std::string& filepath);

};

