#include "yqlua/QsListWidget.h"
#include "yqlua/QsHeaderView.h"
#include "yqlua/define.h"
QsListWidget::QsListWidget(QsWidget* parent):ylib::qt::ListWidget(parent->operator QWidget *())
{
    //std::cout << "[QsTreeWidget] create " << std::hex << this << std::endl;
    QObject::connect(this, &QListWidget::itemClicked, [&](QListWidgetItem* item) {
        if (!m_onItemClicked.isNilref())
            m_onItemClicked(new QsListWidgetItem(item));
    });
}

QsListWidget::~QsListWidget()
{
    //std::cout << "[QsTreeWidget] destory " << std::hex << this << std::endl;
}

void QsListWidget::regist(kaguya::State& T)
{
    T["QsListWidget"].setClass(kaguya::UserdataMetatable<QsListWidget>()
        .setConstructors<QsListWidget(QsWidget*)>()
        .addStaticFunction("toQ", &QsListWidget::toQ)
        .addFunction("setSelectionMode", &QsListWidget::setSelectionMode)
        .addFunction("clear", &QsListWidget::clear)
        .addFunction("onItemClicked", &QsListWidget::onItemClicked)
        .addFunction("setItemWidget", &QsListWidget::setItemWidget)
        .addFunction("currentItem", &QsListWidget::currentItem)
        .addFunction("verticalScrollBar", &QsListWidget::verticalScrollBar)
        .addFunction("horizontalScrollBar", &QsListWidget::horizontalScrollBar)
        .addFunction("addItem", &QsListWidget::addItem)
        .addFunction("remove", &QsListWidget::remove)
        .addFunction("count", &QsListWidget::count)
    );
    QsListWidgetItem::regist(T);
}

void QsListWidget::setItemWidget(QsListWidgetItem* item, QsWidget* widget)
{
    QListWidget::setItemWidget(item->item(),widget->operator QWidget * ());
}
void QsListWidget::onItemClicked(kaguya::LuaFunction callback)
{
    m_onItemClicked = callback;
}

void QsListWidget::setSelectionMode(int mod)
{
    QListWidget::setSelectionMode((QAbstractItemView::SelectionMode)mod);
}
QsScrollBar QsListWidget::verticalScrollBar()
{
    return QsScrollBar(QListWidget::verticalScrollBar());
}
QsScrollBar QsListWidget::horizontalScrollBar()
{
    return QsScrollBar(QListWidget::horizontalScrollBar());
}
QsListWidgetItem QsListWidget::currentItem()
{
    return QsListWidgetItem(QListWidget::currentItem(),nullptr,false);
}
void QsListWidget::remove(QsListWidgetItem item)
{
    delete item.item();
}
void QsListWidget::addItem(QsListWidgetItem item)
{
    QListWidget::addItem(item.item());
}
QsListWidgetItem::QsListWidgetItem(QListWidgetItem* item,QsListWidget* parent, bool  create)
{
    if (item == nullptr)
    {
        if(create)
            m_item = new QListWidgetItem(parent);
    }
    else
    {
        m_item = item;
    }
        
}

QsListWidgetItem::~QsListWidgetItem()
{
}

void QsListWidgetItem::regist(kaguya::State& T)
{
    T["QsListWidgetItem"].setClass(kaguya::UserdataMetatable<QsListWidgetItem>()
        .setConstructors<QsListWidgetItem(QListWidgetItem*,QsListWidget*)>()
        .addFunction("setText", &QsListWidgetItem::setText)
        .addFunction("text", &QsListWidgetItem::text)
        .addFunction("setStringData", &QsListWidgetItem::setStringData)
        .addFunction("setIntData", &QsListWidgetItem::setIntData)
        .addFunction("stringData", &QsListWidgetItem::stringData)
        .addFunction("intData", &QsListWidgetItem::intData)
        .addFunction("setTextColor", &QsListWidgetItem::setTextColor)
        .addFunction("empty", &QsListWidgetItem::empty)
        .addFunction("setHeight", &QsListWidgetItem::setHeight)
        .addFunction("setTextAlignment", &QsListWidgetItem::setTextAlignment)
    );
}

void QsListWidgetItem::setText(const std::string& title)
{
    //m_item->setTextAlignment
    m_item->setText(STDtQ(title));
}

std::string QsListWidgetItem::text()
{
    return QtSTD(m_item->text());
}
void QsListWidgetItem::setStringData(int role, const std::string& data)
{
    m_item->setData(role, STDtQ(data));
}

void QsListWidgetItem::setIntData(int role,int data)
{
    m_item->setData(role, data);
}

std::string QsListWidgetItem::stringData(int role)
{
    return QtSTD(m_item->data(role).toString());
}

int QsListWidgetItem::intData(int role)
{
    return m_item->data(role).toInt();
}
void QsListWidgetItem::setTextColor(const std::string& hex_color_text)
{
    m_item->setTextColor(QColor(STDtQ(hex_color_text)));
}

void QsListWidgetItem::setHeight(int height)
{
    m_item->setSizeHint(QSize(m_item->sizeHint().width(), height));

}
void QsListWidgetItem::setTextAlignment(int value)
{
    m_item->setTextAlignment(value);
}