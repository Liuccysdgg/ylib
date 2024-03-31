
#include "yqlua/QsIcon.h"
QsIcon::QsIcon(const std::string& filepath):QIcon(QString::fromStdString(filepath))
{
    if (filepath.empty())
        std::cout << "QsIcon filepath is empty" << std::endl;
}

QsIcon::~QsIcon()
{ 
}
void QsIcon::regist(kaguya::State& T)
{
    T["QsIcon"].setClass(kaguya::UserdataMetatable<QsIcon>()
        .setConstructors<QsIcon(const std::string&)>()
        .addFunction("setSize",&QsIcon::setSize)
    );
}
void QsIcon::setSize(int width, int height)
{
    QIcon s  = this->pixmap(QSize(width, height));
    (*(QIcon*)(this)) = s;
}
