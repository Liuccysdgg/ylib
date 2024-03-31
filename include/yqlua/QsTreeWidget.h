#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsElement.h"
#include "QsWidget.h"
#include "QsScrollBar.h"

#include "yqt/treewidget.h"

class QsTreeWidget;
class QsHeaderView;
class QsTreeWidgetItem
{
public:
	QsTreeWidgetItem(QTreeWidgetItem* item,QsTreeWidget* parent = nullptr,bool create = true);
	~QsTreeWidgetItem();
	QTreeWidgetItem* item() { return m_item; }
	friend class QsTreeWidget;
	bool empty() { return m_item == nullptr; }
private:
	static void regist(kaguya::State& T);
private:
	void setText(int column, const std::string& title);
	std::string text(int column);
	void addChild(QsTreeWidgetItem* item);
	void setStringData(int column, int role, const std::string& data);
	void setIntData(int column, int role,int data);
	std::string stringData(int column, int role);
	int intData(int column, int role);
	void setExpanded(bool expand);
	void setTextColor(int column, const std::string& hex_color_text);
private:
	QTreeWidgetItem* m_item = nullptr;
};

class QsTreeWidget :public ylib::qt::TreeWidget,public QsElement
{
public:
	QsTreeWidget(QsWidget* parent = nullptr);
	~QsTreeWidget();
	static void regist(kaguya::State& T);

private:
	void setItemWidget(QsTreeWidgetItem* item,int column,QsWidget* widget);
	void addTopLevelItem(QsTreeWidgetItem* item);
	void onItemClicked(kaguya::LuaFunction callback);
	void setSelectionMode(int mod);
	QsScrollBar verticalScrollBar();
	QsScrollBar horizontalScrollBar();
	QsTreeWidgetItem  currentItem();

	QsHeaderView header();
	// 通过 QsElement 继承
	virtual QsWidget* toQ() override{ return appQs(new QsWidget(this)); }
private:
	kaguya::LuaFunction m_onItemClicked;
};



