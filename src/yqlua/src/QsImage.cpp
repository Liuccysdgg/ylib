#include "yqlua/QsImage.h"
#include "yqt/util.h"
#include "yutil/codec.h"
#include "yqlua/QsMovie.h"
QsImage::QsImage(QsWidget* parent):ylib::qt::Label(parent->operator QWidget *())
{
    ::ylib::qt::Label::onMouseRelease([&](QMouseEvent*, ylib::qt::IElement*)->bool {
        if (!m_onMouseRelease.isNilref()) {
            return m_onMouseRelease();
        }
        return false;
    });
}

QsImage::~QsImage()
{
}
void QsImage::regist(kaguya::State& T)
{
    T["QsImage"].setClass(kaguya::UserdataMetatable<QsImage>()
        .setConstructors<QsImage(QsWidget*)>()
        .addStaticFunction("toQ", &QsImage::toQ)
        .addFunction("setImage", &QsImage::setImage)
        .addFunction("setImage2", &QsImage::setImage2)
        .addFunction("onMouseRelease", &QsImage::onMouseRelease)
        .addFunction("setMovie", &QsImage::setMovie)
    );
}

void QsImage::setImage(const std::string& base64image)
{
    ylib::qt::background(this, ylib::codec::base64::de(base64image));
}
void QsImage::setImage2(const QsBuffer& data)
{
    ylib::qt::background(this,data);
}
void QsImage::onMouseRelease(kaguya::LuaFunction callback)
{
    m_onMouseRelease = callback;
}
void QsImage::setMovie(QsMovie* movie)
{
    QLabel::setMovie(movie->movie());
}