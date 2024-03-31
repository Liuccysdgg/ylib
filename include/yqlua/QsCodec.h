#pragma once
#include <string>
#include <Kaguya/kaguya.hpp>
#include "QsBuffer.h"
class QsCodec
{
public:
	QsCodec();
	~QsCodec();
	static void regist(kaguya::State& T);
private:
	static std::string to_utf8(const std::string& value);
	static std::string to_gbk(const std::string& value);

	static QsBuffer base64_de(const std::string& value);
	static std::string base64_en(const QsBuffer& value);

	static QsBuffer aes_en(const QsBuffer& value,const std::string& key);
	static QsBuffer aes_de(const QsBuffer& value, const std::string& key);
};

