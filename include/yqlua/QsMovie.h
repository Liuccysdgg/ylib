#pragma once
#include "QtGui/qicon.h"
#include <string>
#include <Kaguya/kaguya.hpp>
#include "yutil/ini.h"
#include "QtGui/qmovie.h"
class QsMovie
{
public:
	QsMovie(const std::string& filepath);
	~QsMovie();
	static void regist(kaguya::State& T);
	QMovie* movie() { return m_movie; }
private:
	void setSpeed(int speed);
	void start();
private:
	QMovie* m_movie = nullptr;
};

