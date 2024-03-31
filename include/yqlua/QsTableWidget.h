#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsElement.h"
#include "QsWidget.h"
#include "QsIcon.h"
#include "QsHeaderView.h"
#include "yqt/tablewidget.h"
#include "QsScrollBar.h"
class QsTableWidget;
class QsTableWidgetItem 
{
public:
	QsTableWidgetItem(QTableWidgetItem* item = nullptr,QsTableWidget* parent = nullptr, bool create = true);
	~QsTableWidgetItem();
	QTableWidgetItem* item() { return m_item; }
	int row();
	bool empty() { return m_item == nullptr; } 
	void setIcon(QsIcon icon);
	void setColor(const std::string& hex);
	friend class QsTableWidget;
private:
	static void regist(kaguya::State& T);
private:
	void setText(const std::string& text);
private:
	QTableWidgetItem* m_item = nullptr;
};

class QsTableWidget :public ylib::qt::TableWidget,public QsElement
{
public:
	QsTableWidget(QsWidget* parent = nullptr);
	~QsTableWidget();
	static void regist(kaguya::State& T);
private:
	void setSelectionBehavior(int value);
	void setSelectionMode(int value);
	void setEditTriggers(int value);
	void setItem(int row, int column, QsTableWidgetItem* item);
	void setHorizontalHeaderLabels(const kaguya::LuaTable& labels);
	void setCellWidget(int row,int column,QsWidget* widget);
	
	QsHeaderView horizontalHeader();
	QsHeaderView verticalHeader();
	QsScrollBar verticalScrollBar();
	QsScrollBar horizontalScrollBar();


	void onItemClicked(kaguya::LuaFunction callback);
	QsTableWidgetItem  currentItem();

	void setCurrentItem(QsTableWidgetItem item);
	QsTableWidgetItem item(int row,int column);

	kaguya::LuaFunction itemClickedEvent() { return m_onItemClicked; }

	void onLeave(kaguya::LuaFunction callback) { m_onLeave = callback; }
	void onEnter(kaguya::LuaFunction callback) { m_onEnter = callback; }


	void initEvent();
	// 通过 QsElement 继承
	virtual QsWidget* toQ() override{ return appQs(new QsWidget(this)); }
private:
	//bool eventFilter(QObject* object, QEvent* event) override;
private:
	kaguya::LuaFunction m_onItemClicked;

	kaguya::LuaFunction m_onLeave;
	kaguya::LuaFunction m_onEnter;


	QTableWidgetItem* m_lastItem = nullptr;
};



