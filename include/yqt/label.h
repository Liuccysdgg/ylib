#pragma once
#include <functional>
#include "QtWidgets/qlabel.h"
#include "ielement.h"
namespace ylib::qt
{
class Label:public QLabel,public IElement{
public:
    Label(QWidget* parent = nullptr);
    ~Label();
private:
    void enterEvent(QEvent* evt) override{if(!__onEnter(evt)){::QLabel::enterEvent(evt);}}
    void leaveEvent(QEvent* evt) override{if(!__onLeave(evt)){::QLabel::leaveEvent(evt);}}
    void mousePressEvent(QMouseEvent* e) override{if(!__onMousePress(e)){::QLabel::mousePressEvent(e);}}
    void mouseReleaseEvent(QMouseEvent* e) override{if(!__onMouseRelease(e)){::QLabel::mouseReleaseEvent(e);}}
    void mouseMoveEvent(QMouseEvent* e) override{if(!__onMouseMove(e)){::QLabel::mouseMoveEvent(e);}}
};

}
