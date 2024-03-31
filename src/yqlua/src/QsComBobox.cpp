
#include "yqlua/QsComBobox.h"
#include "yqlua/define.h"
QsComBobox::QsComBobox(QsWidget* parent):ylib::qt::ComboBox(parent->operator QWidget *())
{
}

QsComBobox::~QsComBobox()
{ 
}
void QsComBobox::regist(kaguya::State& T)
{
    T["QsComBobox"].setClass(kaguya::UserdataMetatable<QsComBobox>()
        .setConstructors<QsComBobox(QsWidget*)>()
        .addStaticFunction("toQ", &QsComBobox::toQ)
        .addFunction("addItem", &QsComBobox::addItem)
        .addFunction("setCurrentIndex", &QsComBobox::setCurrentIndex)
        .addFunction("currentIndex", &QsComBobox::currentIndex)
        
    );
}

void QsComBobox::addItem(const std::string& text)
{
    ylib::qt::ComboBox::addItem(STDtQ(text));
}
