#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsElement.h"
#include "QsWidget.h"
#include "QsBuffer.h"
#include "yqt/label.h"

class QsLabel :public ylib::qt::Label,public QsElement
{
public:
	QsLabel(QsWidget* parent = nullptr);
	~QsLabel();
	static void regist(kaguya::State& T);
private:
	void onMouseRelease(kaguya::LuaFunction callback) { m_onMouseRelease = callback; }
	void setText(const std::string& title);
	void setAlignment(int value);
	std::string text();
	void setBackgroundImage(const QsBuffer& data);
	// 根据文本自动设置label宽度
	void setAutoWidthForFont();
	
private:
	kaguya::LuaFunction m_onMouseRelease;
	// 通过 QsElement 继承
	virtual QsWidget* toQ() override{ return appQs(new QsWidget(this)); }
};

