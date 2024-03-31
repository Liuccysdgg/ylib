#include "yqlua/QsWidget.h"
#include <QtWidgets/qdesktopwidget.h>
#include <QtWidgets/qapplication.h>
#include <QtGui/qicon.h>
#include <QtWidgets/qmessagebox.h>
#include "yqlua/QsHBoxLayout.h"
#include "yqlua/QsVBoxLayout.h"
#include "yutil/window.h"
#include "yutil/codec.h"
#include "yqlua/define.h"
QsWidget::QsWidget(QWidget* point, QWidget* parent)
{
    if (point == nullptr)
    {
        m_auto_delete = true;
        m_qwidget = (QWidget*)new QsWidgetPrivatte(parent);
    }
    else
        m_qwidget = point;

    //std::cout << "[QsWidget] create " << std::hex << this << std::endl;
}

QsWidget::~QsWidget()
{
    //std::cout << "[QsWidget] destory " << std::hex << this << std::endl;
    if (m_auto_delete)
    {
        delete (QsWidgetPrivatte*)m_qwidget;
    }
}

void QsWidget::regist(kaguya::State& T)
{
    T["QsWidget"].setClass(kaguya::UserdataMetatable<QsWidget>()
        .setConstructors<QsWidget(QWidget*, QWidget*)>()
        .addStaticFunction("destory", &QsWidget::destory)
        .addFunction("setWindowTitle", &QsWidget::setWindowTitle)
        .addFunction("setWindowModality", &QsWidget::setWindowModality)
        .addFunction("setWindowMouseMoveHeight", &QsWidget::setWindowMouseMoveHeight)
        .addFunction("windowFlags", &QsWidget::windowFlags)
        .addFunction("setWindowFlags", &QsWidget::setWindowFlags)
        .addFunction("setFixedHeight", &QsWidget::setFixedHeight)
        .addFunction("setFixedWidth", &QsWidget::setFixedWidth)
        .addFunction("setFixedSize", &QsWidget::setFixedSize)
        .addFunction("setStyleSheet", &QsWidget::setStyleSheet)
        .addFunction("setParent", &QsWidget::setParent)
        .addFunction("setGeometry", &QsWidget::setGeometry)
        .addFunction("show", &QsWidget::show)
        .addFunction("hide", &QsWidget::hide)
        .addFunction("x", &QsWidget::x)
        .addFunction("y", &QsWidget::y)
        .addFunction("width", &QsWidget::width)
        .addFunction("height", &QsWidget::height)
        .addFunction("isPage", &QsWidget::isPage)
        .addFunction("raise", &QsWidget::raise)
        .addFunction("lower", &QsWidget::lower)
        .addFunction("setEnabled", &QsWidget::setEnabled)
        .addFunction("hideTopBar", &QsWidget::hideTopBar)
        .addFunction("showMinimized", &QsWidget::showMinimized)
        .addFunction("exec_ex", &QsWidget::exec_ex)
        .addFunction("close", &QsWidget::close)
        .addFunction("info", &QsWidget::info)
        .addFunction("error", &QsWidget::error)
        .addFunction("warn", &QsWidget::warn)
        .addFunction("isHidden", &QsWidget::isHidden)
        .addFunction("toWidgetCenter", &QsWidget::toWidgetCenter)
        .addFunction("winId", &QsWidget::winId)
        .addFunction("center", &QsWidget::center)
        .addFunction("setWindowIcon", &QsWidget::setWindowIcon)
        .addFunction("setWindowIcon2", &QsWidget::setWindowIcon2)
        .addFunction("move", &QsWidget::move)
        .addFunction("setWindowState", &QsWidget::setWindowState)
        .addFunction("qwidget", &QsWidget::qwidget)
        .addFunction("qobject", &QsWidget::qobject)
        .addFunction("setAttribute", &QsWidget::setAttribute)
    );
   
}
void QsWidget::setWindowTitle(const std::string& title)
{
    //Qt::WA_TranslucentBackground
    //m_qwidget->isHidden()
    m_qwidget->setWindowTitle(QString::fromStdString(title));
}
void QsWidget::setWindowMouseMoveHeight(int height)
{
    if (m_auto_delete)
    {
        ((QsWidgetPrivatte*)m_qwidget)->setMouseMoveHeight(height);
    }
}
void QsWidget::setWindowModality(int mod)
{
    m_qwidget->setWindowModality((Qt::WindowModality)mod);
}
void QsWidget::setFixedHeight(int height)
{
    m_qwidget->setFixedHeight(height);
}
void QsWidget::setFixedWidth(int width)
{
   // m_qwidget->setWindowIcon(QIcon(,));
    m_qwidget->setFixedWidth(width);
}

void QsWidget::setFixedSize(int width, int height)
{
    m_qwidget->setFixedSize(QSize(width,height));
}

void QsWidget::setStyleSheet(const std::string& qss)
{
    m_qwidget->setStyleSheet(QString::fromStdString(qss));
}

void QsWidget::setParent(QsWidget* parent)
{
    m_qwidget->setParent(parent->m_qwidget);
}
void QsWidget::setAttribute(int value)
{
    m_qwidget->setAttribute((Qt::WidgetAttribute)value);
}
int QsWidget::x()
{
    return m_qwidget->x();
}

int QsWidget::y()
{
    return m_qwidget->y();
}

int QsWidget::width()
{
    return m_qwidget->width();
}

int QsWidget::height()
{
    return m_qwidget->height();
}

void QsWidget::setGeometry(int x, int y, int width, int height)
{
    m_qwidget->setGeometry(x, y, width, height);
}

int QsWidget::windowFlags()
{
    return (int)m_qwidget->windowFlags();
}

void QsWidget::setWindowFlags(int flags)
{
    m_qwidget->setWindowFlags((Qt::WindowFlags)flags);
}

void QsWidget::setEnabled(bool enable)
{
    m_qwidget->setEnabled(enable);
}

void QsWidget::raise()
{
    m_qwidget->raise();
}

void QsWidget::lower()
{
    m_qwidget->lower();
}

void QsWidget::hideTopBar()
{
    m_qwidget->setWindowFlags(Qt::FramelessWindowHint | m_qwidget->windowFlags());
}

void QsWidget::showMinimized()
{
    m_qwidget->showMinimized();
}

void QsWidget::close()
{
    m_qwidget->close();
}

void QsWidget::exec_ex()
{
    if (m_auto_delete)
    {
        ((QsWidgetPrivatte*)m_qwidget)->exec_ex();
        return;
    }
    m_qwidget->show();
}

void QsWidget::info(const std::string& body)
{
    QMessageBox::information(m_qwidget, "提示", STDtQ(body));
}

void QsWidget::error(const std::string& body)
{
    QMessageBox::critical(m_qwidget, "提示", STDtQ(body));
}

void QsWidget::warn(const std::string& body)
{
    QMessageBox::warning(m_qwidget, "提示", STDtQ(body));
}

bool QsWidget::isHidden()
{
    return m_qwidget->isHidden();
}

void QsWidget::toWidgetCenter(QsWidget* widget)
{
    int x = widget->m_qwidget->geometry().center().x() - m_qwidget->width() / 2;
    int y = widget->m_qwidget->geometry().center().y() - m_qwidget->height() / 2;
    m_qwidget->move(x, y);
}

void QsWidget::center()
{
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width() - m_qwidget->width()) / 2;
    int y = (screenGeometry.height() - m_qwidget->height()) / 2;
    // 将窗口移动到屏幕中央
    m_qwidget->move(x, y);
}

int QsWidget::winId()
{
    return (int)m_qwidget->winId();
}
void QsWidget::setWindowIcon2(const std::string& base64image)
{
    auto imageBuffer = ylib::codec::base64::de(base64image);
    QByteArray arr;
    arr.append(imageBuffer.data(), imageBuffer.length());
    QPixmap pixmap;
    pixmap.loadFromData(arr);
    m_qwidget->setWindowIcon(QIcon(pixmap));
}
void QsWidget::setWindowIcon(QsIcon icon)
{
    m_qwidget->setWindowIcon(icon);
}
void QsWidget::setWindowState(int state)
{
    m_qwidget->setWindowState((Qt::WindowState)state);
}
void QsWidget::setLayoutV(QsVBoxLayout* layout)
{
    m_qwidget->setLayout(layout->layout());
}
void QsWidget::setLayoutH(QsHBoxLayout* layout)
{
    m_qwidget->setLayout(layout->layout());
}
void QsWidget::show()
{
    m_qwidget->show();
}

void QsWidget::hide()
{
    m_qwidget->hide();
}
void QsWidget::move(int x, int y)
{
    m_qwidget->move(x,y);
}
QsWidgetPrivatte::QsWidgetPrivatte(QWidget* parent):ylib::qt::Widget<QWidget>(parent)
{
    initEvent();
}

QsWidgetPrivatte::~QsWidgetPrivatte()
{
}

void QsWidgetPrivatte::initEvent()
{
    
}

void QsWidgetPrivatte::exec_ex()
{
    show();
    m_loop.exec();
}

void QsWidgetPrivatte::closeEvent(QCloseEvent* event)
{
    m_loop.quit();
    QWidget::closeEvent(event);
}

