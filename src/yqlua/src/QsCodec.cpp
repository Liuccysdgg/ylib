
#include "yqlua/QsCodec.h"
#include "yutil/codec.h"
QsCodec::QsCodec()
{
}

QsCodec::~QsCodec()
{
}

void QsCodec::regist(kaguya::State& T)
{
    T["QsCodec"].setClass(kaguya::UserdataMetatable<QsCodec>()
        .addStaticFunction("to_utf8", &QsCodec::to_utf8)
        .addStaticFunction("to_gbk", &QsCodec::to_gbk)
        .addStaticFunction("base64_de", &QsCodec::base64_de)
        .addStaticFunction("base64_en", &QsCodec::base64_en)
        .addStaticFunction("aes_en", &QsCodec::aes_en)
        .addStaticFunction("aes_de", &QsCodec::aes_de)
    );
}

std::string QsCodec::to_utf8(const std::string& value)
{
    return ylib::codec::to_utf8(value);
}

std::string QsCodec::to_gbk(const std::string& value)
{
    return ylib::codec::to_gbk(value);
}

QsBuffer QsCodec::base64_de(const std::string& value)
{
    QsBuffer result;
    *((ylib::buffer*)&result) = ylib::codec::base64::de(value);
    return result;
}
std::string QsCodec::base64_en(const QsBuffer& value)
{
    return ylib::codec::base64::en(value);
}

QsBuffer QsCodec::aes_en(const QsBuffer& value, const std::string& key)
{
    QsBuffer result;
    *((ylib::buffer*)&result) = ylib::codec::aes::en(value, key, codec::aes::variant::AES128,codec::aes::mode::CBC);
    return result;
}
QsBuffer QsCodec::aes_de(const QsBuffer& value, const std::string& key)
{
    QsBuffer result;
    *((ylib::buffer*)&result) = ylib::codec::aes::de(value, key, codec::aes::variant::AES128, codec::aes::mode::CBC);
    return result;
}