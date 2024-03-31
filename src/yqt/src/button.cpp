#include "yqt/button.h"
#include "yqt/util.h"
#include "ybase/exception.h"
ylib::qt::Button::Button(QWidget* parent):QPushButton(parent)
{

}

ylib::qt::Button::~Button()
{

}
void ylib::qt::Button::setImages(const std::vector<buffer> &images)
{
    if(images.size() != 4){
        throw ylib::exception("the number of incoming images is not equal to 4, param size:"+std::to_string(images.size()));
    }
    m_images = images;

    // set bg image
    {
        if (this->isEnabled())
        {
            if (!m_images[0].empty())
                ylib::qt::background(this, m_images[0]);
        }
        else
        {
            if (!m_images[3].empty())
                ylib::qt::background(this, m_images[3]);
            else
                ylib::qt::background(this, m_images[0]);
        }
    }
}

void ylib::qt::Button::enterEvent(QEvent *evt)
{
    if(m_images.size() == 4 && !m_images[1].empty())
        ylib::qt::background(this,m_images[1]);

    if(!__onEnter(evt)){
        ::QPushButton::enterEvent(evt);
    }
}

void ylib::qt::Button::leaveEvent(QEvent *evt)
{
    if(m_images.size() == 4 && !m_images[0].empty())
        ylib::qt::background(this,m_images[0]);

    if(!__onLeave(evt)){
        ::QPushButton::leaveEvent(evt);
    }
}

void ylib::qt::Button::mousePressEvent(QMouseEvent *e)
{
    if(m_images.size() == 4 && !m_images[2].empty())
        ylib::qt::background(this,m_images[2]);

   if(!__onMousePress(e)){
            ::QPushButton::mousePressEvent(e);
        }
}

void ylib::qt::Button::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_images.size() == 4 && !m_images[0].empty())
        ylib::qt::background(this,m_images[0]);

    if(!__onMouseRelease(e)){
        ::QPushButton::mouseReleaseEvent(e);
    }
}

void ylib::qt::Button::mouseMoveEvent(QMouseEvent *e)
{
    if(!__onMouseMove(e)){
        ::QPushButton::mouseMoveEvent(e);
    }
}
