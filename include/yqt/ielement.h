#pragma once
#include "QtGui/qevent.h"
#include <map>
#include <any>
namespace ylib::qt {
    class IElement{ 
    public:
        ylib::qt::IElement* getIElement(){return this;}
        void onMousePress(std::function<bool(QMouseEvent*,ylib::qt::IElement*)> callback);
        void onMouseRelease(std::function<bool(QMouseEvent*,ylib::qt::IElement*)> callback);
        void onMouseMove(std::function<bool(QMouseEvent*,ylib::qt::IElement*)> callback);
        void onEnter(std::function<bool(QEvent*,ylib::qt::IElement*)> callback);
        void onLeave(std::function<bool(QEvent*,ylib::qt::IElement*)> callback);

        std::any extra(const std::string& name);
        void extra(const std::string& name,const std::any& value);

        virtual void __do() {}
    protected:
        bool __onMousePress(QMouseEvent* e);
        bool __onMouseRelease(QMouseEvent* e);
        bool __onMouseMove(QMouseEvent* e);
        bool __onEnter(QEvent* e);
        bool __onLeave(QEvent* e);
    private:
        std::vector<std::function<bool(QMouseEvent*,ylib::qt::IElement*)>> m_mousePressCallback;
        std::vector<std::function<bool(QMouseEvent*,ylib::qt::IElement*)>> m_mouseReleaseCallback;
        std::vector<std::function<bool(QMouseEvent*,ylib::qt::IElement*)>> m_mouseMoveCallback;
        std::vector<std::function<bool(QEvent*,ylib::qt::IElement*)>> m_enterCallback;
        std::vector<std::function<bool(QEvent*,ylib::qt::IElement*)>> m_leaveCallback;
        std::map<std::string, std::any> m_extra;
    };
}
