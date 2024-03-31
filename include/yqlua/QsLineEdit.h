#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsElement.h"
#include "QsWidget.h"
#include "QsBuffer.h"
#include "QsIcon.h"
#include "yqt/lineedit.h"
class QsAction;
class QsLineEdit :public ylib::qt::LineEdit,public QsElement
{
public:
	QsLineEdit(QsWidget* parent = nullptr);
	~QsLineEdit();
	static void regist(kaguya::State& T);
private:
	void setText(const std::string& title);
	void setPlaceholderText(const std::string& value);
	void setEchoMode(int value);
	void setReadOnly(bool onlyread);
	void setMaxLength(int length);
	std::string text();
	void addAction(QsAction* action,bool left);

	void onReturnPressed(kaguya::LuaFunction callback) { m_returnPressedCallback = callback; }
	void onMouseRelease(kaguya::LuaFunction callback) { m_onMouseRelease = callback; }
private:
	kaguya::LuaFunction m_returnPressedCallback;
	kaguya::LuaFunction m_onMouseRelease;
	kaguya::LuaFunction m_onIconAction;
	// 通过 QsElement 继承
	virtual QsWidget* toQ() override{ return appQs(new QsWidget(this)); }
};

