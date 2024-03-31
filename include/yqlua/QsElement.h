#pragma once
#include <vector>
#include "QsWidget.h"

class QsElement
{
public:
	QsElement() {}
	~QsElement() {
		for (size_t i = 0; i < m_frees.size(); i++)
			delete m_frees[i];
	}
	virtual QsWidget* toQ() = 0;
	QsWidget* appQs(QsWidget* widget) { m_frees.push_back(widget); return widget; };

	void setExtra(const std::string& name, const std::string& value) {
		m_extra[name] = value;
	}
	std::string extra(const std::string& name) {
		return m_extra[name];
	}
private:
	std::vector<QsWidget*> m_frees;
	std::map<std::string, std::string> m_extra;
};