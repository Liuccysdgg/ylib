#pragma once
#include <functional>
#include "QtWidgets/qlineedit.h"
#include "ielement.h"
namespace ylib::qt
{
    class LineEdit:public QLineEdit,public IElement{
    public:
        LineEdit(QWidget* parent = nullptr);
        ~LineEdit();
    private:
        void enterEvent(QEvent* evt) override{if(!__onEnter(evt)){::QLineEdit::enterEvent(evt);}}
        void leaveEvent(QEvent* evt) override{if(!__onLeave(evt)){::QLineEdit::leaveEvent(evt);}}
        void mousePressEvent(QMouseEvent* e) override{if(!__onMousePress(e)){::QLineEdit::mousePressEvent(e);}}
        void mouseReleaseEvent(QMouseEvent* e) override{if(!__onMouseRelease(e)){::QLineEdit::mouseReleaseEvent(e);}}
        void mouseMoveEvent(QMouseEvent* e) override{if(!__onMouseMove(e)){::QLineEdit::mouseMoveEvent(e);}}
    };

}
