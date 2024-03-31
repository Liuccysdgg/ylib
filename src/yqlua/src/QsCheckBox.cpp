#include "yqlua/QsCheckBox.h"
#include "yqlua/define.h"
QsCheckBox::QsCheckBox(QsWidget* parent):QCheckBox(parent->operator QWidget *())
{
    QObject::connect(this, &QCheckBox::stateChanged, [&](int) {
        if (!m_stateChangedCallback.isNilref())
            m_stateChangedCallback(this->isChecked());
    });
}

QsCheckBox::~QsCheckBox()
{
}
void QsCheckBox::regist(kaguya::State& T)
{
    T["QsCheckBox"].setClass(kaguya::UserdataMetatable<QsCheckBox>()
        .setConstructors<QsCheckBox(QsWidget*)>()
        .addStaticFunction("toQ", &QsCheckBox::toQ)
        .addFunction("onStateChanged", &QsCheckBox::onStateChanged)
        .addFunction("setText", &QsCheckBox::setText)
        .addFunction("setChecked", &QsCheckBox::setChecked)
        .addFunction("isChecked", &QsCheckBox::isChecked)
        .addFunction("setIcon", &QsCheckBox::setIcon)
        .addFunction("setIconSize", &QsCheckBox::setIconSize)
        
        
    );
}
void QsCheckBox::setText(const std::string& title)
{
    QCheckBox::setText(STDtQ(title));
}
void QsCheckBox::setIcon(QsIcon icon)
{
    QCheckBox::setIcon(icon);
}
void QsCheckBox::setIconSize(int width, int height)
{
    QCheckBox::setIconSize(QSize(width, height));
}