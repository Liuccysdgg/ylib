#pragma once
#include <functional>
#include "QtWidgets/qprogressbar.h"
#include "ielement.h"
namespace ylib::qt
{
class ProgressBar:public QProgressBar,public IElement{
public:
    ProgressBar(QWidget* parent = nullptr);
    ~ProgressBar();
private:
    void enterEvent(QEvent* evt) override{if(!__onEnter(evt)){::QProgressBar::enterEvent(evt);}}
    void leaveEvent(QEvent* evt) override{if(!__onLeave(evt)){::QProgressBar::leaveEvent(evt);}}
    void mousePressEvent(QMouseEvent* e) override{if(!__onMousePress(e)){::QProgressBar::mousePressEvent(e);}}
    void mouseReleaseEvent(QMouseEvent* e) override{if(!__onMouseRelease(e)){::QProgressBar::mouseReleaseEvent(e);}}
    void mouseMoveEvent(QMouseEvent* e) override{if(!__onMouseMove(e)){::QProgressBar::mouseMoveEvent(e);}}
};

}
