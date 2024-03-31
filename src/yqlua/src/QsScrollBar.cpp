#include "yqlua/QsScrollBar.h"
QsScrollBar::QsScrollBar(QScrollBar* bar)
{
    m_bar = bar;
}

QsScrollBar::~QsScrollBar()
{
}

void QsScrollBar::regist(kaguya::State& T)
{
    T["QsScrollBar"].setClass(kaguya::UserdataMetatable<QsScrollBar>()
        .setConstructors<QsScrollBar(QScrollBar*)>()
        .addStaticFunction("toQ", &QsScrollBar::toQ)
    );
}
