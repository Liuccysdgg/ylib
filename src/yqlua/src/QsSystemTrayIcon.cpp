
#include "yqlua/QsSystemTrayIcon.h"
QsSystemTrayIcon::QsSystemTrayIcon(QsWidget* widget)
{
    m_tray = new QSystemTrayIcon(widget->operator QWidget *());
    QObject::connect(m_tray, &QSystemTrayIcon::activated, [&](QSystemTrayIcon::ActivationReason reason) {
        if (!m_onActivated.isNilref())
            m_onActivated((int)reason);
    });
}

QsSystemTrayIcon::~QsSystemTrayIcon()
{ 
}
void QsSystemTrayIcon::regist(kaguya::State& T)
{
    T["QsSystemTrayIcon"].setClass(kaguya::UserdataMetatable<QsSystemTrayIcon>()
        .setConstructors<QsSystemTrayIcon(QsWidget*)>()
        .addFunction("hide", &QsSystemTrayIcon::hide)
        .addFunction("show", &QsSystemTrayIcon::show)
        .addFunction("setIcon", &QsSystemTrayIcon::setIcon)
        .addFunction("setToolTip", &QsSystemTrayIcon::setToolTip)
        .addFunction("onActivated", &QsSystemTrayIcon::onActivated)
    );
}
void QsSystemTrayIcon::hide()
{
    m_tray->hide();
}
void QsSystemTrayIcon::show()
{
    m_tray->show();
}
void QsSystemTrayIcon::setIcon(const QsIcon& icon)
{
    m_tray->setIcon(icon);
}
void QsSystemTrayIcon::setToolTip(const std::string& msg)
{
    m_tray->setToolTip(QString::fromStdString(msg));
}