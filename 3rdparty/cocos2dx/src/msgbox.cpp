#include "msgbox.h"
#include "define.h"
#include "c2d.h"
void ylib::c2d::MsgBox::show(const std::string& text, std::function<void(ylib::c2d::MsgBox::MessageResult)> callback, const std::string& ok, const std::string& cancel)
{
    if (this->isVisible())
        return;

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


    m_value->setString(text);
    m_btn_ok_label->setString(ok);
    m_btn_cancel_label->setString(cancel);
    if (cancel.empty())
    {
        m_btn_cancel->setVisible(false);
        m_btn_ok->setPosition(m_cancel_position);
    }
    else
    {
        m_btn_cancel->setVisible(true);
        m_btn_ok->setPosition(m_ok_position);
    }
    this->setVisible(true);



    m_callback = callback;
    m_btn_ok->on_clicked([&](ylib::c2d::Button* btn) {
        getScene()->getEventDispatcher()->removeEventListener(m_listener);
        if (m_callback != nullptr)
            m_callback(MR_OK);
        this->setVisible(false);
    });
    m_btn_cancel->on_clicked([&](ylib::c2d::Button* btn) {
        getScene()->getEventDispatcher()->removeEventListener(m_listener);
        if (m_callback != nullptr)
            m_callback(MR_CANCEL);
        this->setVisible(false);
    });
}
bool ylib::c2d::MsgBox::init()
{



    return true;
}
ylib::c2d::MsgBox::MsgBox()
{
}

ylib::c2d::MsgBox::~MsgBox()
{
}
ylib::c2d::MsgBox* ylib::c2d::MsgBox::create()
{
    CREATE_C2D_CLASS(ylib::c2d::MsgBox);
}

void ylib::c2d::MsgBox::load(
    const std::string& background,
    const std::array<std::string, 2> btn_ok,
    cocos2d::Vec2 btn_ok_poisition,
    const std::array<std::string, 2> btn_cancel,
    cocos2d::Vec2 btn_cancel_poisition,
    const std::string& text_font_famliy,
    int text_font_size,
    const std::string& text_font_color,
    const std::string& btn_font_famliy,
    int btn_font_size,
    const std::string& btn_font_color
)
{
#if 1
    m_ok_position = btn_ok_poisition;
    m_cancel_position = btn_cancel_poisition;
    auto v2 = cocos2d::Director::getInstance()->getVisibleSize();

    // 创建一个全屏的半透明黑色层作为遮罩
    this->initWithColor(cocos2d::Color4B(0, 0, 0, 100), v2.width, v2.height);
    this->m_background = cocos2d::Sprite::create(background);
    this->addChild(this->m_background);

    center(this->m_background);

    this->m_value = cocos2d::Label::createWithTTF("", text_font_famliy, text_font_size);
    this->m_value->setTextColor(toC4B(text_font_color));
    //this->m_value->setDimensions(340, this->m_value->getHeight());

    this->m_value->setAlignment(cocos2d::TextHAlignment::CENTER);  // 设置水平居中对齐

    center(m_value, m_background);
    m_background->addChild(m_value);






    // 确认
    {
        m_btn_ok = c2d::Button::create(btn_ok[0], btn_ok[1]);
        m_btn_ok->setPosition(m_ok_position);
        m_btn_ok_label = cocos2d::Label::createWithTTF("", btn_font_famliy, btn_font_size);
        m_btn_ok_label->setTextColor(toC4B(btn_font_color));
        m_btn_ok_label->setString("确认");
        m_btn_ok->addChild(m_btn_ok_label);
        center(m_btn_ok_label, m_btn_ok);
        m_btn_ok_label->setPositionY(m_btn_ok_label->getPositionY() + 1);
        m_background->addChild(m_btn_ok);


        
    }
    // 取消
    {
        m_btn_cancel = c2d::Button::create(btn_cancel[0], btn_cancel[1]);
        m_btn_cancel->setPosition(m_cancel_position);
        m_btn_cancel_label = cocos2d::Label::createWithTTF("", btn_font_famliy, btn_font_size);
        m_btn_cancel_label->setTextColor(toC4B(btn_font_color));
        m_btn_cancel_label->setString("取消");
        m_btn_cancel->addChild(m_btn_cancel_label);
        center(m_btn_cancel_label, m_btn_cancel);
        m_btn_cancel_label->setPositionY(m_btn_cancel_label->getPositionY() + 1);
        m_background->addChild(m_btn_cancel);
    }

    this->setVisible(false);
    this->setLocalZOrder(9998);
#endif
}