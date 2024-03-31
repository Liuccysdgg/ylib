#include "yutil/file.h"
#include "yqlua/QsBuffer.h"
QsBuffer::QsBuffer()
{
}

QsBuffer::~QsBuffer()
{ 
}
void QsBuffer::regist(kaguya::State& T)
{
    T["QsBuffer"].setClass(kaguya::UserdataMetatable<QsBuffer>()
        .setConstructors<QsBuffer()>()
        .addFunction("append", &QsBuffer::append)
        .addFunction("saveToFile", &QsBuffer::saveToFile)
        .addFunction("clear", &QsBuffer::clear)
        .addFunction("to_string", &QsBuffer::to_string)
        .addFunction("length", &QsBuffer::length)
        .addStaticFunction("fromString", &QsBuffer::fromString)
    );
}
QsBuffer QsBuffer::fromString(const std::string& value)
{
    QsBuffer result;
    *(ylib::buffer*)&result = value;
    return result;
}
void QsBuffer::append(unsigned long long data_point, unsigned int length)
{
    ylib::buffer::append((const char*)data_point, length);
}

bool QsBuffer::saveToFile(const std::string& filepath)
{
    return ylib::file::write(filepath,*this);
}
