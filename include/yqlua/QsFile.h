#pragma once
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsBuffer.h"
class QsFile
{
public:
	QsFile();
	~QsFile();
	static void regist(kaguya::State& T);
private:
	static bool create_dir(const std::string& dirpath);
	static bool remove_dir(const std::string& dirpath);
	static bool remove(const std::string& filepath);
	static bool exist(const std::string& filepath);
	static size_t size(const std::string& filepath);
	static bool write(const std::string& filepath,const QsBuffer& data);
};

