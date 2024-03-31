#pragma once
#include <string>
#include <vector>
#include <Kaguya/kaguya.hpp>
class QsSystem
{
public:
	QsSystem();
	~QsSystem();
	static void regist(kaguya::State& T);
private:
	static std::string current_dir();
	static std::string mac();
	static void notification(const std::string& title, const std::string& message);
	static void open_browser(const std::string& url);
};

