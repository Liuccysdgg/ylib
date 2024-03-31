#include "yqlua/QsApp.h"
#include "QtWidgets/qapplication.h"
#include "yqt/util.h"
QsApp::QsApp()
{
    //std::cout << "[QsApp] create " << std::hex << this << std::endl;
}

QsApp::~QsApp()
{
   // std::cout << "[QsApp] destory " << std::hex << this << std::endl;
}

void QsApp::regist(kaguya::State& T)
{
    T["QsApp"].setClass(kaguya::UserdataMetatable<QsApp>()
        .addStaticFunction("quit", &QsApp::quit)
        .addStaticFunction("read_res", &QsApp::read_res)
    );
}
    

void QsApp::quit()
{
    QApplication::quit();
}

QsBuffer QsApp::read_res(const std::string& filepath)
{
    QsBuffer result;
    *(ylib::buffer*)(&result) = qt::read(QString::fromStdString(filepath));
    return result;
}
