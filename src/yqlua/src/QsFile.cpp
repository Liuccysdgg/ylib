
#include "yqlua/QsFile.h"
#include "yutil/file.h"
QsFile::QsFile()
{
}

QsFile::~QsFile()
{
}

void QsFile::regist(kaguya::State& T)
{
    T["QsFile"].setClass(kaguya::UserdataMetatable<QsFile>()
        .addStaticFunction("create_dir", &QsFile::create_dir)
        .addStaticFunction("remove_dir", &QsFile::remove_dir)
        .addStaticFunction("exist", &QsFile::exist)
        .addStaticFunction("size", &QsFile::size)
        .addStaticFunction("remove", &QsFile::remove)
        .addStaticFunction("write", &QsFile::write)
    );
}

bool QsFile::create_dir(const std::string& dirpath)
{
    return ylib::file::create_dir(dirpath);
}

bool QsFile::remove_dir(const std::string& dirpath)
{
    return ylib::file::remove_dir(dirpath);
}

bool QsFile::remove(const std::string& filepath)
{
    return ylib::file::remove(filepath);
}
bool QsFile::exist(const std::string& filepath)
{
    return ylib::file::exist(filepath);
}

size_t QsFile::size(const std::string& filepath)
{
    return ylib::file::size(filepath);
}
bool QsFile::write(const std::string& filepath, const QsBuffer& data)
{
    return ylib::file::write(filepath, data);
}