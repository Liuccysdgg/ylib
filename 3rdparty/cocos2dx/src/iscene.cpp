#include "IScene.h"
#include "msgbox.h"
using namespace cocos2d;
void ylib::c2d::IScene::openVirtualTopBar(uint32 height)
{
    m_topBar.height = height;
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = [&](EventMouse* event) {
        EventMouse* e = (EventMouse*)event;
        auto position = e->getLocationInView();
        if (position.y >= Director::getInstance()->getWinSize().height - m_topBar.height) {
            m_topBar.dragginng = true;
            m_topBar.mouse_position = position;
        }
    };
    mouseListener->onMouseMove = [&](EventMouse* event) {
        if (m_topBar.dragginng) {
            EventMouse* e = (EventMouse*)event;
            auto position = e->getLocation();  // 屏幕坐标

            HWND hwnd = Director::getInstance()->getOpenGLView()->getWin32Window();
            RECT rect;
            if (GetWindowRect(hwnd, &rect)) {
                CCLOG("窗口坐标: 左上角(%ld, %ld), 右下角(%ld, %ld)\n", rect.left, rect.top, rect.right, rect.bottom);
            }
            else {
                CCLOG("获取窗口坐标失败\n");
            }

            int newX = rect.left + position.x - m_topBar.mouse_position.x;
            int newY = rect.top + position.y - (Director::getInstance()->getWinSize().height-m_topBar.mouse_position.y);
            SetWindowPos(hwnd, NULL, newX, newY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);



            CCLOG("%f\t%f", position.x, position.y);
        }
        };
    mouseListener->onMouseUp = [&](EventMouse* event) {
        m_topBar.dragginng = false;
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

}
bool ylib::c2d::IScene::init()
{
    /// 信息窗
    m_msgBox = c2d::MsgBox::create();
    this->addChild(m_msgBox->toC2D(), 9998);


    return initScene(m_msgBox);
}
void ylib::c2d::IScene::recv_msg(int type, int action, const ylib::json& data)
{

}
ylib::c2d::IScene::IScene()
{
}

ylib::c2d::IScene::~IScene()
{
}

void ylib::c2d::IScene::messageBox(const std::string& text, std::function<void(ylib::c2d::MsgBox::MessageResult)> callback, const std::string& ok, const std::string& cancel)
{
    m_msgBox->show(text,callback,ok,cancel);
}

void ylib::c2d::IScene::push_msg(int type, int action, const ylib::json& data)
{
    cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
        this->recv_msg(type,action,data);
    });
}
