
#include "yqlua/QsLabel.h"

#include "QtGui/qfontmetrics.h"

#include "yqlua/define.h"

#include "yqt/util.h"
QsLabel::QsLabel(QsWidget* parent):ylib::qt::Label(parent->operator QWidget *())
{
    ylib::qt::Label::onMouseRelease([&](QMouseEvent* e, ylib::qt::IElement*)->bool {
        if (m_onMouseRelease.isNilref())
            return true;
        return m_onMouseRelease((int)e->button(), this);
    });
}

QsLabel::~QsLabel()
{ 
}
void QsLabel::regist(kaguya::State& T)
{
    T["QsLabel"].setClass(kaguya::UserdataMetatable<QsLabel>()
        .setConstructors<QsLabel(QsWidget*)>()
        .addStaticFunction("toQ", &QsLabel::toQ)
        .addFunction("onMouseRelease", &QsLabel::onMouseRelease)
        .addFunction("setText", &QsLabel::setText)
        .addFunction("setAlignment", &QsLabel::setAlignment)
        .addFunction("text", &QsLabel::text)
        .addFunction("setBackgroundImage", &QsLabel::setBackgroundImage)
        .addFunction("setAutoWidthForFont", &QsLabel::setAutoWidthForFont)

    );
}
void QsLabel::setText(const std::string& title)
{
    ylib::qt::Label::setText(STDtQ(title));
    
}
void QsLabel::setAlignment(int value)
{
    ylib::qt::Label::setAlignment((Qt::Alignment)value);
}
std::string QsLabel::text()
{
    return QtSTD(ylib::qt::Label::text());
}
void QsLabel::setBackgroundImage(const QsBuffer& data)
{
    qt::background(this, data);
}
void QsLabel::setAutoWidthForFont()
{
    // 创建字体度量对象以测量文本的字宽
    QFontMetrics fontMetrics(this->font());
    auto textWidth = fontMetrics.boundingRect(ylib::qt::Label::text()).width();
    setFixedWidth(textWidth);
}
