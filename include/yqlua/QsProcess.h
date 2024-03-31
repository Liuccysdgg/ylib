#pragma once
#include <string>
#include <vector>
#include <Kaguya/kaguya.hpp>
class QsProcess
{
public:
	QsProcess();
	~QsProcess();
	static void regist(kaguya::State& T);
private:
	static bool create(const std::string& filepath);
};

