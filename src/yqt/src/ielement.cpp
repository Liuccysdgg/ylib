#pragma once
#include "yqt/ielement.h"

void ylib::qt::IElement::onMousePress(std::function<bool(QMouseEvent*,ylib::qt::IElement*)> callback){
    m_mousePressCallback.push_back(callback);
}
void ylib::qt::IElement::onMouseRelease(std::function<bool(QMouseEvent*,ylib::qt::IElement*)> callback){
    m_mouseReleaseCallback.push_back(callback);
};
void ylib::qt::IElement::onMouseMove(std::function<bool(QMouseEvent*,ylib::qt::IElement*)> callback){
    m_mouseMoveCallback.push_back(callback);
};
void ylib::qt::IElement::onEnter(std::function<bool(QEvent*,ylib::qt::IElement*)> callback){
    m_enterCallback.push_back(callback);
};
void ylib::qt::IElement::onLeave(std::function<bool(QEvent*,ylib::qt::IElement*)> callback){
    m_leaveCallback.push_back(callback);
};
std::any ylib::qt::IElement::extra(const std::string& name)
{
    return m_extra[name];
}
void ylib::qt::IElement::extra(const std::string& name, const std::any& value)
{
    m_extra[name] = value;
}
bool ylib::qt::IElement::__onMousePress(QMouseEvent* e){
    bool result = false;
    for(size_t i=0;i<m_mousePressCallback.size();i++)
    {
        result = m_mousePressCallback[i](e,this);
        if(result)
            break;
    }
    return result;
}
bool ylib::qt::IElement::__onMouseRelease(QMouseEvent* e){
    bool result = false;
    for(size_t i=0;i<m_mouseReleaseCallback.size();i++)
    {
        result = m_mouseReleaseCallback[i](e,this);
        if(result)
            break;
    }
    return result;
}
bool ylib::qt::IElement::__onMouseMove(QMouseEvent* e){
    bool result = false;
    for(size_t i=0;i<m_mouseMoveCallback.size();i++)
    {
        result = m_mouseMoveCallback[i](e,this);
        if(result)
            break;
    }
    return result;
}
bool ylib::qt::IElement::__onEnter(QEvent* e){
    bool result = false;
    for(size_t i=0;i<m_enterCallback.size();i++)
    {
        result = m_enterCallback[i](e,this);
        if(result)
            break;
    }
    return result;
}
bool ylib::qt::IElement::__onLeave(QEvent* e){
    bool result = false;
    for(size_t i=0;i<m_leaveCallback.size();i++)
    {
        result = m_leaveCallback[i](e,this);
        if(result)
            break;
    }
    return result;
}
