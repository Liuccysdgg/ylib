#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsElement.h"
#include "QsWidget.h"
#include "QsScrollBar.h"

#include "yqt/listwidget.h"

class QsListWidget;
class QsHeaderView;
class QsListWidgetItem
{
public:
	QsListWidgetItem(QListWidgetItem* item, QsListWidget* parent = nullptr,bool create = true);
	~QsListWidgetItem();
	QListWidgetItem* item() { return m_item; }
	friend class QsListWidget;
	bool empty() { return m_item == nullptr; }
private:
	static void regist(kaguya::State& T);
private:
	void setText(const std::string& title);
	std::string text();
	void setIntData(int role,int data);
	void setStringData(int role, const std::string& data);
	std::string stringData(int role);
	int intData(int role);
	void setExpanded(bool expand);
	void setTextColor(const std::string& hex_color_text);
	void setHeight(int height);
	void setTextAlignment(int value);
private:
	QListWidgetItem* m_item = nullptr;
};

class QsListWidget :public ylib::qt::ListWidget,public QsElement
{
public:
	QsListWidget(QsWidget* parent = nullptr);
	~QsListWidget();
	static void regist(kaguya::State& T);

private:
	void setItemWidget(QsListWidgetItem* item,QsWidget* widget);
	void onItemClicked(kaguya::LuaFunction callback);
	void setSelectionMode(int mod);
	QsScrollBar verticalScrollBar();
	QsScrollBar horizontalScrollBar();
	QsListWidgetItem currentItem();
	void remove(QsListWidgetItem item);
	void addItem(QsListWidgetItem item);

	
	// 通过 QsElement 继承
	virtual QsWidget* toQ() override{ return appQs(new QsWidget(this)); }
private:
	kaguya::LuaFunction m_onItemClicked;
};



