#pragma once
#include <functional>
#include "QtWidgets/qwidget.h"
#include "QtWidgets/qdialog.h"
#include "ielement.h"
#include "yutil/window.h"
namespace ylib::qt 
{
template<typename T>
class Widget:public T,public IElement{
public: 
    Widget(QWidget* parent = nullptr):T(parent){
        ylib::qt::IElement::onMouseMove([&](QMouseEvent* e, ylib::qt::IElement*)->bool {
            auto getTaskBarRect = []() {
                ylib::Rect rect;
                auto hwndTaskbar = ylib::window::find("Shell_TrayWnd");
                return ylib::window::rect(hwndTaskbar);
                };
            //若鼠标左键被按下
            if (m_mousePres)
            {
                // std::cout << "MouseMove" << std::endl;
                //不让鼠标移动到任务栏下面
                if (e->globalPos().y() > (int)getTaskBarRect().top)
                {
                    QCursor::setPos(QPoint((int)e->globalPos().x(), (int)getTaskBarRect().top));
                }
                //鼠标相对于屏幕的位置
                QPoint movePos = e->globalPos();
                //移动主窗体位置
                this->move(movePos - m_movePoint);
            }
            return false;
        });
        ylib::qt::IElement::onMouseRelease([&](QMouseEvent* e, ylib::qt::IElement*)->bool {
            if (e->button() == Qt::LeftButton)
            {
                m_mousePres = false;
            }
            return false;
        });
        ylib::qt::IElement::onMousePress([&](QMouseEvent* e, ylib::qt::IElement*)->bool {
            if (e->button() == Qt::LeftButton && e->pos().y() < (int)m_top_height)
            {
                m_movePoint = e->pos();
                m_mousePres = true;
            }
            return false;
        });

    }
    ~Widget(){}
    inline void setMouseMoveHeight(int height) { m_top_height = height; }
private:
    void enterEvent(QEvent* evt) override{if(!__onEnter(evt)){::QWidget::enterEvent(evt);}}
    void leaveEvent(QEvent* evt) override{if(!__onLeave(evt)){::QWidget::leaveEvent(evt);}}
    void mousePressEvent(QMouseEvent* e) override{if(!__onMousePress(e)){::QWidget::mousePressEvent(e);}}
    void mouseReleaseEvent(QMouseEvent* e) override{if(!__onMouseRelease(e)){::QWidget::mouseReleaseEvent(e);}}
    void mouseMoveEvent(QMouseEvent* e) override{if(!__onMouseMove(e)){::QWidget::mouseMoveEvent(e);}}
private:

    // 移动
    QPoint m_movePoint;
    bool m_mousePres = false;
    uint32 m_top_height = 0;
};

}
