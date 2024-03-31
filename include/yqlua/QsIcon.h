#pragma once
#include "QtGui/qicon.h"
#include <string>
#include <Kaguya/kaguya.hpp>
#include "yutil/ini.h"

class QsIcon :public QIcon
{
public:
	QsIcon(const std::string& filepath);
	~QsIcon();
	static void regist(kaguya::State& T);
private:
	void setSize(int width,int height);
};

