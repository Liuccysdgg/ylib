
#include "yqlua/QsProcess.h"
#include "yutil/process.h"
#include <Windows.h>
QsProcess::QsProcess()
{
}

QsProcess::~QsProcess()
{
}

void QsProcess::regist(kaguya::State& T)
{
    T["QsProcess"].setClass(kaguya::UserdataMetatable<QsProcess>()
        .addStaticFunction("create", &QsProcess::create)
    );  
}

bool QsProcess::create(const std::string& filepath)
{
    return process::create(filepath);
}

