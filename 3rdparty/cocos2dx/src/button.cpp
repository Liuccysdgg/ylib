#include "button.h"
ylib::c2d::Button::Button(const std::string& normal, const std::string& clicked):m_image_normal(normal),m_image_clicked(clicked)
{
    
}

bool ylib::c2d::Button::init()
{
    if (!::cocos2d::ui::Button::init(m_image_normal, m_image_clicked))
        return false;
    this->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
        if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
            if (m_callback != nullptr)
                m_callback(this);
        }
    });
    return true;
}

ylib::c2d::Button::~Button()
{

}

ylib::c2d::Button* ylib::c2d::Button::create(const std::string& normal, const std::string& clicked)
{
    ylib::c2d::Button* p = new (std::nothrow) ylib::c2d::Button(normal,clicked);
    if (p && p->init())
    {
        p->autorelease();
        return p;

    }
    CC_SAFE_DELETE(p);
    return nullptr;
}
