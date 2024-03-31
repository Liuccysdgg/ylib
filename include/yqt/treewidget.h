#pragma once
#include <functional>
#include "QtWidgets/qtreewidget.h"
#include "ielement.h"
namespace ylib::qt
{
    class TreeWidget:public QTreeWidget,public IElement{
    public:
        TreeWidget(QWidget* parent = nullptr);
        ~TreeWidget();
    private:
        void enterEvent(QEvent* evt) override{if(!__onEnter(evt)){::QTreeWidget::enterEvent(evt);}}
        void leaveEvent(QEvent* evt) override{if(!__onLeave(evt)){::QTreeWidget::leaveEvent(evt);}}
        void mousePressEvent(QMouseEvent* e) override{if(!__onMousePress(e)){::QTreeWidget::mousePressEvent(e);}}
        void mouseReleaseEvent(QMouseEvent* e) override{if(!__onMouseRelease(e)){::QTreeWidget::mouseReleaseEvent(e);}}
        void mouseMoveEvent(QMouseEvent* e) override{if(!__onMouseMove(e)){::QTreeWidget::mouseMoveEvent(e);}}
    };

}
