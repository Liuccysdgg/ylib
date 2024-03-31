
#include "yqlua/QsProgressbar.h"
QsProgressbar::QsProgressbar(QsWidget* parent):ylib::qt::ProgressBar(parent->operator QWidget *())
{
}

QsProgressbar::~QsProgressbar()
{ 
}
void QsProgressbar::regist(kaguya::State& T) 
{
    T["QsProgressbar"].setClass(kaguya::UserdataMetatable<QsProgressbar>()
        .setConstructors<QsProgressbar(QsWidget*)>()
        .addStaticFunction("toQ", &QsProgressbar::toQ)
        .addFunction("setValue", &QsProgressbar::setValue)
        .addFunction("setMinimum", &QsProgressbar::setMinimum)
        .addFunction("setMaximum", &QsProgressbar::setMaximum)
        .addFunction("setTextVisible", &QsProgressbar::setTextVisible)
    );
}
