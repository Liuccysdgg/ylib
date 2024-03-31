#include "yqlua/QsVBoxLayout.h"
QsVBoxLayout::QsVBoxLayout()
{
    m_layout = new QVBoxLayout();
    m_layout->setSpacing(0);
    m_layout->setMargin(0);
}

QsVBoxLayout::~QsVBoxLayout()
{
}

void QsVBoxLayout::regist(kaguya::State& T)
{
    T["QsVBoxLayout"].setClass(kaguya::UserdataMetatable<QsVBoxLayout>()
        .setConstructors<QsVBoxLayout()>()
        .addFunction("addStretch", &QsVBoxLayout::addStretch)
    );
}

void QsVBoxLayout::addStretch()
{
    m_layout->addStretch();
}

void QsVBoxLayout::addWidget(QsWidget* widget)
{
    m_layout->addWidget(widget->operator QWidget * ());
}
