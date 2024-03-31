#include "yqlua/QsHeaderView.h"
QsHeaderView::QsHeaderView(QHeaderView* view)
{
    m_view = view;
}

QsHeaderView::~QsHeaderView()
{
}

void QsHeaderView::regist(kaguya::State& T)
{
    T["QsHeaderView"].setClass(kaguya::UserdataMetatable<QsHeaderView>()
        .setConstructors<QsHeaderView(QHeaderView*)>()
        .addStaticFunction("toQ", &QsHeaderView::toQ)
        .addFunction("setSectionResizeMode", &QsHeaderView::setSectionResizeMode)
    );
}

void QsHeaderView::setSectionResizeMode(int logicalIndex, int mode)
{
    m_view->setSectionResizeMode(logicalIndex, (QHeaderView::ResizeMode)mode);
}
