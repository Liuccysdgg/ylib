
#include "yqlua/QsIni.h"
QsIni::QsIni()
{
}

QsIni::~QsIni()
{ 
}
void QsIni::regist(kaguya::State& T)
{
    T["QsIni"].setClass(kaguya::UserdataMetatable<QsIni>()
        .setConstructors<QsIni()>()
        .addFunction("open", &QsIni::open)
        .addFunction("read", &QsIni::read)
        .addFunction("write", &QsIni::write)
    );
}

std::string QsIni::read(const std::string& name, const std::string& node)
{
    std::string result = ylib::ini::read(name, node);
    return result;
}
