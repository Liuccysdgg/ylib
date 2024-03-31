#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsElement.h"
#include "QsWidget.h"
#include "QsBuffer.h"
#include "yqt/label.h"
class QsMovie;
class QsImage :public ylib::qt::Label,public QsElement
{
public:
	QsImage(QsWidget* parent = nullptr);
	~QsImage();
	static void regist(kaguya::State& T);
private:
	void setImage(const std::string& base64image);
	void setImage2(const QsBuffer& data);
	void onMouseRelease(kaguya::LuaFunction callback);
	void setMovie(QsMovie* movie);
private:
	// 通过 QsElement 继承
	virtual QsWidget* toQ() override{ return appQs(new QsWidget(this)); }
private:
	kaguya::LuaFunction m_onMouseRelease;
};

