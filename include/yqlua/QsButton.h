#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsElement.h"
#include "QsWidget.h"
#include "QsIcon.h"
#include "yqt/button.h"

class QsButton :public QsElement
{
public:
	QsButton(QsWidget* parent = nullptr);
	~QsButton();
	static void regist(kaguya::State& T);
private:
	void onMouseRelease(kaguya::LuaFunction callback);
	void setText(const std::string& title);
	void setImages(const std::string& img1, const std::string& img2, const std::string& img3, const std::string& img4);
	void setImagesFromRes(const std::string& img1, const std::string& img2, const std::string& img3, const std::string& img4);
	void setIcon(const QsIcon& icon);
private:
	kaguya::LuaFunction m_onMouseRelease;
	ylib::qt::Button* m_button = nullptr;
	// 通过 QsElement 继承
	virtual QsWidget* toQ() override{ return appQs(new QsWidget(m_button)); }

};

