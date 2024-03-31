#pragma once
#include <QtWidgets/qwidget.h>
#include "QtGui/qevent.h"
#include "QtCore/qeventloop.h"
#include <string>
#include <Kaguya/kaguya.hpp>
#include "ybase/define.h"
#include "yqt/widget.hpp"
#include "yqlua/QsIcon.h"
class QsWidgetPrivatte:public ylib::qt::Widget<QWidget>
{
public:
	QsWidgetPrivatte(QWidget* parent);
	~QsWidgetPrivatte();
	
	void exec_ex();
private:
	void initEvent();
protected:
	void closeEvent(QCloseEvent* event);
private:
	QEventLoop m_loop;
};
class QsVBoxLayout;
class QsHBoxLayout;
class QsWidget
{
public:
	QsWidget(QWidget* point = nullptr, QWidget* parent = nullptr);
	~QsWidget();
	operator QWidget* () const { return m_qwidget; }
	static void regist(kaguya::State& T);
private:
	static void destory(QWidget* widget) { if (widget == nullptr)return; widget->hide(); widget->setParent(nullptr); }
private:
	QWidget* qwidget() { return m_qwidget; }
	QObject* qobject() { return m_qwidget; }
	void setWindowTitle(const std::string& title);
	void setWindowMouseMoveHeight(int height);
	void setWindowModality(int mod);
	void setFixedHeight(int height);
	void setFixedWidth(int width);
	void setFixedSize(int width,int height);
	void setStyleSheet(const std::string& qss);
	void setParent(QsWidget* parent);
	void setAttribute(int value);
	void show();
	void hide();
	int x();
	int y();
	int width();
	int height();
	void setGeometry(int x, int y, int width, int height);
	bool isPage() { return m_auto_delete; }
	int windowFlags();
	void setWindowFlags(int flags);

	void setEnabled(bool enable);
	void raise();
	void lower();
	void hideTopBar();
	void showMinimized();
	void close();
	void exec_ex();

	void info(const std::string& body);
	void error(const std::string& body);
	void warn(const std::string& body);

	bool isHidden();

	void toWidgetCenter(QsWidget* widget);

	void center();

	int winId();

	void setWindowIcon2(const std::string& base64image);
	void setWindowIcon(QsIcon icon);
	void setWindowState(int state);

	void setLayoutV(QsVBoxLayout* layout);
	void setLayoutH(QsHBoxLayout* layout);

	void move(int x, int y);
private:
	kaguya::LuaFunction m_onMousePress;
	QWidget* m_qwidget = nullptr;
	bool m_auto_delete = false;

};

