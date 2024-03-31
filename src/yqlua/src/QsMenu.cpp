#include "yqlua/QsMenu.h"

QsAction::QsAction()
{
    m_action = new QAction();

    QObject::connect(m_action, &QAction::triggered, [&]() {
        if (!m_onTriggered.isNilref())
            m_onTriggered();
    });
}
QsAction::~QsAction()
{

}
void QsAction::regist(kaguya::State& T)
{
    T["QsAction"].setClass(kaguya::UserdataMetatable<QsAction>()
        .setConstructors<QsAction()>()
        .addFunction("setText", &QsAction::setText)
        .addFunction("onTriggered", &QsAction::onTriggered)
        .addFunction("setIcon", &QsAction::setIcon)
    );
}
void QsAction::setText(const std::string& text)
{
    m_action->setText(QString::fromStdString(text));
}
void QsAction::setIcon(QsIcon icon)
{
    m_action->setIcon(icon);
}
QsMenu::QsMenu(QsWidget* parent)
{
    m_menu = new QMenu(parent!=nullptr?parent->operator QWidget *():nullptr);
}
QsMenu::~QsMenu()
{
   
}
void QsMenu::regist(kaguya::State& T)
{
    T["QsMenu"].setClass(kaguya::UserdataMetatable<QsMenu>()
        .setConstructors<QsMenu()>()
        .addFunction("addAction", &QsMenu::addAction)
        .addFunction("exec", &QsMenu::exec)
        .addFunction("toQ", &QsMenu::toQ)
    );
}
void QsMenu::addAction(QsAction* action)
{
    m_menu->addAction(action->action());
} 
void QsMenu::exec()
{
    m_menu->exec(QCursor::pos());
}