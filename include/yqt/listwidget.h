#pragma once
#include <functional>
#include "QtWidgets/qlistwidget.h"
#include "ielement.h"
namespace ylib::qt
{
    class ListWidget:public QListWidget,public IElement{
    public:
        ListWidget(QWidget* parent = nullptr);
        ~ListWidget();
    private:
        void enterEvent(QEvent* evt) override{if(!__onEnter(evt)){::QListWidget::enterEvent(evt);}}
        void leaveEvent(QEvent* evt) override{if(!__onLeave(evt)){::QListWidget::leaveEvent(evt);}}
        void mousePressEvent(QMouseEvent* e) override{if(!__onMousePress(e)){::QListWidget::mousePressEvent(e);}}
        void mouseReleaseEvent(QMouseEvent* e) override{if(!__onMouseRelease(e)){::QListWidget::mouseReleaseEvent(e);}}
        void mouseMoveEvent(QMouseEvent* e) override{if(!__onMouseMove(e)){::QListWidget::mouseMoveEvent(e);}}
    };

}
