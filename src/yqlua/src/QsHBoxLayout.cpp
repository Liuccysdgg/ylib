#include "yqlua/QsHBoxLayout.h"
QsHBoxLayout::QsHBoxLayout()
{
    m_layout = new QHBoxLayout();
    m_layout->setSpacing(0);
    m_layout->setMargin(0);
}

QsHBoxLayout::~QsHBoxLayout()
{
}

void QsHBoxLayout::regist(kaguya::State& T)
{
    T["QsHBoxLayout"].setClass(kaguya::UserdataMetatable<QsHBoxLayout>()
        .setConstructors<QsHBoxLayout()>()
        .addFunction("addStretch",&QsHBoxLayout::addStretch)
    );
}

void QsHBoxLayout::addStretch()
{
    m_layout->addStretch();
}

void QsHBoxLayout::addWidget(QsWidget* widget)
{
    m_layout->addWidget(widget->operator QWidget * ());
}
