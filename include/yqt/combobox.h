#pragma once
#include <functional>
#include "QtWidgets/qcombobox.h"
#include "ielement.h"
namespace ylib::qt
{
class ComboBox:public QComboBox,public IElement{
public:
    ComboBox(QWidget* parent = nullptr);
    ~ComboBox();
private:
    void enterEvent(QEvent* evt) override{ if (!__onEnter(evt)) { ::QComboBox::enterEvent(evt);}}
    void leaveEvent(QEvent* evt) override{ if (!__onLeave(evt)) { ::QComboBox::leaveEvent(evt);}}
    void mousePressEvent(QMouseEvent* e) override{ if (!__onMousePress(e)) { ::QComboBox::mousePressEvent(e);}}
    void mouseReleaseEvent(QMouseEvent* e) override{ if (!__onMouseRelease(e)) { ::QComboBox::mouseReleaseEvent(e);}}
    void mouseMoveEvent(QMouseEvent* e) override{ if (!__onMouseMove(e)) { ::QComboBox::mouseMoveEvent(e);}}
};

}
