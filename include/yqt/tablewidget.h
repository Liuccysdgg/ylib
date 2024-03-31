#pragma once
#include <functional>
#include "QtWidgets/qtablewidget.h"
#include "ielement.h"
namespace ylib::qt
{
class TableWidget:public QTableWidget,public IElement{
public:
    TableWidget(QWidget* parent = nullptr);
    ~TableWidget();
private:
    void enterEvent(QEvent* evt) override{if(!__onEnter(evt)){::QTableWidget::enterEvent(evt);}}
    void leaveEvent(QEvent* evt) override{if(!__onLeave(evt)){::QTableWidget::leaveEvent(evt);}}
    void mousePressEvent(QMouseEvent* e) override{if(!__onMousePress(e)){::QTableWidget::mousePressEvent(e);}}
    void mouseReleaseEvent(QMouseEvent* e) override{if(!__onMouseRelease(e)){::QTableWidget::mouseReleaseEvent(e);}}
    void mouseMoveEvent(QMouseEvent* e) override{if(!__onMouseMove(e)){::QTableWidget::mouseMoveEvent(e);}}
};

}
