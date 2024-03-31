#include "yqlua/QsLineEdit.h"
#include "QtWidgets/qaction.h"
#include "yqlua/define.h"
#include "yqlua/QsMenu.h"
QsLineEdit::QsLineEdit(QsWidget* parent):ylib::qt::LineEdit(parent->operator QWidget *())
{
    QObject::connect(this, &QLineEdit::returnPressed, [&]() {
        if (!m_returnPressedCallback.isNilref())
            m_returnPressedCallback();
    });
    ylib::qt::LineEdit::onMouseRelease([&](QMouseEvent* event, ylib::qt::IElement*)->bool {
        std::cout << event->pos().x() << "\t" << event->pos().y() << std::endl;
        if (!m_onMouseRelease.isNilref())
        {
            m_onMouseRelease();
            return true;
        }
        return false;
            });
}

QsLineEdit::~QsLineEdit()
{
   // std::cout << "[QsLabel] destory " << std::hex << this << std::endl;
}
void QsLineEdit::regist(kaguya::State& T)
{
    T["QsLineEdit"].setClass(kaguya::UserdataMetatable<QsLineEdit>()
        .setConstructors<QsLineEdit(QsWidget*)>()
        .addStaticFunction("toQ", &QsLineEdit::toQ)
        .addFunction("setText", &QsLineEdit::setText)
        .addFunction("setPlaceholderText", &QsLineEdit::setPlaceholderText)
        .addFunction("setEchoMode", &QsLineEdit::setEchoMode)
        .addFunction("setReadOnly", &QsLineEdit::setReadOnly)
        .addFunction("setMaxLength", &QsLineEdit::setMaxLength)
        .addFunction("text", &QsLineEdit::text)
        .addFunction("addAction", &QsLineEdit::addAction)
        .addFunction("onReturnPressed", &QsLineEdit::onReturnPressed)
    );
}
void QsLineEdit::setText(const std::string& title)
{
    ylib::qt::LineEdit::setText(STDtQ(title));
}

void QsLineEdit::setPlaceholderText(const std::string& value)
{
    ylib::qt::LineEdit::setPlaceholderText(STDtQ(value));
}

void QsLineEdit::setEchoMode(int value)
{
    ylib::qt::LineEdit::setEchoMode((QLineEdit::EchoMode)value);
}

void QsLineEdit::setReadOnly(bool onlyread)
{
    ylib::qt::LineEdit::setReadOnly(onlyread);
}

void QsLineEdit::setMaxLength(int length)
{
    ylib::qt::LineEdit::setMaxLength(length);
}

std::string QsLineEdit::text()
{
    return QtSTD(ylib::qt::LineEdit::text());
}
void QsLineEdit::addAction(QsAction* action, bool left)
{
    // 将动作添加到 QLineEdit
    ylib::qt::LineEdit::addAction(action->action(), left == true ? QLineEdit::LeadingPosition : QLineEdit::TrailingPosition);
}
