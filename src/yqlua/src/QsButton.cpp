#include "yqlua/QsButton.h"
#include "yqlua/define.h"
#include "yutil/codec.h"
#include "yqt/util.h"
QsButton::QsButton(QsWidget* parent)
{
    m_button = new ylib::qt::Button(parent==nullptr?nullptr:parent->operator QWidget *());
    m_button->onMouseRelease([&](QMouseEvent* e, ylib::qt::IElement*)->bool {
        if (m_onMouseRelease.isNilref())
            return true;
        return m_onMouseRelease((int)e->button(),this);
    });
}

QsButton::~QsButton()
{
}

void QsButton::regist(kaguya::State& T)
{
    T["QsButton"].setClass(kaguya::UserdataMetatable<QsButton>()
        .setConstructors<QsButton(QsWidget*)>()
        .addStaticFunction("toQ", &QsButton::toQ)
        .addFunction("onMouseRelease", &QsButton::onMouseRelease)
        .addFunction("setText", &QsButton::setText)
        .addFunction("setImages", &QsButton::setImages)
        .addFunction("setImagesFromRes", &QsButton::setImagesFromRes)
        .addFunction("extra", &QsButton::extra)
        .addFunction("setExtra", &QsButton::setExtra)
        .addFunction("setIcon", &QsButton::setIcon)
    );
}
void QsButton::onMouseRelease(kaguya::LuaFunction callback)
{
    m_onMouseRelease = callback;
}

void QsButton::setText(const std::string& title)
{
    m_button->setText(STDtQ(title));
}

void QsButton::setImages(const std::string& img1, const std::string& img2, const std::string& img3, const std::string& img4)
{
    std::vector<ylib::buffer> images;
    images.resize(4);
    images[0] = codec::base64::de(img1);
    images[1] = codec::base64::de(img2);
    images[2] = codec::base64::de(img3);
    images[3] = codec::base64::de(img4);
    m_button->setImages(images);
}
void QsButton::setImagesFromRes(const std::string& img1, const std::string& img2, const std::string& img3, const std::string& img4)
{
    std::vector<ylib::buffer> images;
    images.resize(4);
    images[0] = qt::read(QString::fromStdString(img1));
    images[1] = qt::read(QString::fromStdString(img2));
    images[2] = qt::read(QString::fromStdString(img3));
    images[3] = qt::read(QString::fromStdString(img4));
    m_button->setImages(images);
}
void QsButton::setIcon(const QsIcon& icon)
{
    m_button->setIcon(icon);
}