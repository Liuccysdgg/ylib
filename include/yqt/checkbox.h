#pragma once
#include <functional>
#include "QtWidgets/qcheckbox.h"
#include "ielement.h"
namespace ylib::qt
{
    class CheckBox :public QCheckBox, public IElement {
    public:
        CheckBox(QWidget* parent = nullptr);
        ~CheckBox();
    private:
        void enterEvent(QEvent* evt) override { if (!__onEnter(evt)) { ::QCheckBox::enterEvent(evt); } }
        void leaveEvent(QEvent* evt) override { if (!__onLeave(evt)) { ::QCheckBox::leaveEvent(evt); } }
        void mousePressEvent(QMouseEvent* e) override { if (!__onMousePress(e)) { ::QCheckBox::mousePressEvent(e); } }
        void mouseReleaseEvent(QMouseEvent* e) override { if (!__onMouseRelease(e)) { ::QCheckBox::mouseReleaseEvent(e); } }
        void mouseMoveEvent(QMouseEvent* e) override { if (!__onMouseMove(e)) { ::QCheckBox::mouseMoveEvent(e); } }
    };

}
