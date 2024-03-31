#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <Kaguya/kaguya.hpp>
#include "yutil/json.h"
#include "yqlua/QsBuffer.h"
class QsWidget;
class QsSkin
{
	struct ResData {
		ylib::buffer data;
		std::string filename;
	};
public:
	QsSkin();
	~QsSkin();
	static void regist(kaguya::State& T);
private:
	bool load(const std::string& filepath);
	QsWidget* widget(const std::string& name);
	std::string config(const std::string& name);
	int pageSize();
	std::string pageName(int index);
	bool existRes(const std::string& name);
	QsBuffer getResData(const std::string& name);
	std::string getResFilename(const std::string& name);
private:
	std::vector<std::string> m_widget_names;
	std::map<std::string,QsWidget*> m_widgets;
	std::map<std::string,ylib::json*> m_widgets_config;
	std::map<std::string, std::shared_ptr<ResData>> m_res;
	ylib::json m_init_json;
};


