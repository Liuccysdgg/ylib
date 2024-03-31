#pragma once
#include <functional>
#include "QtWidgets/qpushbutton.h"

#include "ybase/buffer.h"
#include "ielement.h"
namespace ylib::qt
{
class Button:public QPushButton,public IElement{
public:
    Button(QWidget* parent = nullptr);
    ~Button();
    void setImages(const std::vector<ylib::buffer> &images);
private:
    void enterEvent(QEvent* evt) override;
    void leaveEvent(QEvent* evt) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    std::vector<ylib::buffer> m_images;
};

}
