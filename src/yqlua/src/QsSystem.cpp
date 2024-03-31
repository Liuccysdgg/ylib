
#include "yqlua/QsSystem.h"
#include "yutil/system.h"

#include <Windows.h>
QsSystem::QsSystem()
{
}

QsSystem::~QsSystem()
{
}

void QsSystem::regist(kaguya::State& T)
{
    T["QsSystem"].setClass(kaguya::UserdataMetatable<QsSystem>()
        .addStaticFunction("current_dir", &QsSystem::current_dir)
        .addStaticFunction("mac", &QsSystem::mac)
        .addStaticFunction("notification", &QsSystem::notification)
        .addStaticFunction("open_browser", &QsSystem::open_browser)
    );  
}

std::string QsSystem::current_dir()
{
    return system::current_dir();

    
}
std::string QsSystem::mac()
{
    auto macs = system::mac();
    return macs.size() == 0 ? "" : macs[0];
}
void QsSystem::notification(const std::string& title, const std::string& message)
{
    system::notification(title, message);
}
void QsSystem::open_browser(const std::string& url)
{
    system::open_browser(url);
}

