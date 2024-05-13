#include "dialog.h"
#include "define.h"
ylib::c2d::Dialog::~Dialog()
{
}

ylib::c2d::Dialog* ylib::c2d::Dialog::create()
{
	CREATE_C2D_CLASS(ylib::c2d::Dialog);
}

void ylib::c2d::Dialog::show()
{
    // 销毁事件监听器
    if (m_listener != nullptr)
        getScene()->getEventDispatcher()->removeEventListener(m_listener);
    m_listener = nullptr;
    // 创建事件监听器
    m_listener = cocos2d::EventListenerTouchOneByOne::create();
    m_listener->setSwallowTouches(true);
    m_listener->onTouchBegan = [&](cocos2d::Touch* touch, cocos2d::Event* event) -> bool {
        return isVisible();
        };
    getScene()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_listener, this);
    this->setVisible(true);
}

void ylib::c2d::Dialog::hide()
{
    getScene()->getEventDispatcher()->removeEventListener(m_listener);
    this->setVisible(false);
}

ylib::c2d::Dialog::Dialog()
{
}

bool ylib::c2d::Dialog::init()
{
    // 创建一个全屏的半透明黑色层作为遮罩
    auto v2 = cocos2d::Director::getInstance()->getVisibleSize();
    this->initWithColor(cocos2d::Color4B(0, 0, 0, 100), v2.width, v2.height);

    this->setVisible(false);
	return true;
}
