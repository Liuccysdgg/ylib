#include "yqlua/QsTreeWidget.h"
#include "yqlua/QsHeaderView.h"
#include "yqlua/define.h"
QsTreeWidget::QsTreeWidget(QsWidget* parent):ylib::qt::TreeWidget(parent->operator QWidget *())
{
    //std::cout << "[QsTreeWidget] create " << std::hex << this << std::endl;
    QObject::connect(this, &QTreeWidget::itemClicked, [&](QTreeWidgetItem* item, int column) {
        if (!m_onItemClicked.isNilref())
            m_onItemClicked(new QsTreeWidgetItem(item), column);
    });
}

QsTreeWidget::~QsTreeWidget()
{
    //std::cout << "[QsTreeWidget] destory " << std::hex << this << std::endl;
}

void QsTreeWidget::regist(kaguya::State& T)
{
    T["QsTreeWidget"].setClass(kaguya::UserdataMetatable<QsTreeWidget>()
        .setConstructors<QsTreeWidget(QsWidget*)>()
        .addStaticFunction("toQ", &QsTreeWidget::toQ)
        .addFunction("setRootIsDecorated", &QsTreeWidget::setRootIsDecorated)
        .addFunction("setSelectionMode", &QsTreeWidget::setSelectionMode)
        .addFunction("setColumnCount", &QsTreeWidget::setColumnCount)
        .addFunction("clear", &QsTreeWidget::clear)
        .addFunction("header", &QsTreeWidget::header)
        .addFunction("onItemClicked", &QsTreeWidget::onItemClicked)
        .addFunction("setItemWidget", &QsTreeWidget::setItemWidget)
        .addFunction("addTopLevelItem", &QsTreeWidget::addTopLevelItem)
        .addFunction("currentItem", &QsTreeWidget::currentItem)
        .addFunction("verticalScrollBar", &QsTreeWidget::verticalScrollBar)
        .addFunction("horizontalScrollBar", &QsTreeWidget::horizontalScrollBar)
    );
    QsTreeWidgetItem::regist(T);
}

void QsTreeWidget::setItemWidget(QsTreeWidgetItem* item, int column, QsWidget* widget)
{
    QTreeWidget::setItemWidget(item->item(), column, widget->operator QWidget * ());
}

void QsTreeWidget::addTopLevelItem(QsTreeWidgetItem* item)
{
    QTreeWidget::addTopLevelItem(item->item());

}

void QsTreeWidget::onItemClicked(kaguya::LuaFunction callback)
{
    m_onItemClicked = callback;
}

void QsTreeWidget::setSelectionMode(int mod)
{
    QTreeWidget::setSelectionMode((QAbstractItemView::SelectionMode)mod);
}
QsScrollBar QsTreeWidget::verticalScrollBar()
{
    return QsScrollBar(QTreeWidget::verticalScrollBar());
}
QsScrollBar QsTreeWidget::horizontalScrollBar()
{
    return QsScrollBar(QTreeWidget::horizontalScrollBar());
}
QsTreeWidgetItem QsTreeWidget::currentItem()
{
    return QsTreeWidgetItem(QTreeWidget::currentItem(),nullptr,false);
}


QsHeaderView QsTreeWidget::header()
{
    return QsHeaderView(QTreeWidget::header());
}

QsTreeWidgetItem::QsTreeWidgetItem(QTreeWidgetItem* item,QsTreeWidget* parent, bool  create)
{
    if (item == nullptr)
    {
        if(create)
            m_item = new QTreeWidgetItem(parent);
    }
    else
    {
        m_item = item;
    }
        
}

QsTreeWidgetItem::~QsTreeWidgetItem()
{
}

void QsTreeWidgetItem::regist(kaguya::State& T)
{
    T["QsTreeWidgetItem"].setClass(kaguya::UserdataMetatable<QsTreeWidgetItem>()
        .setConstructors<QsTreeWidgetItem(QTreeWidgetItem*,QsTreeWidget*)>()
        .addFunction("setText", &QsTreeWidgetItem::setText)
        .addFunction("text", &QsTreeWidgetItem::text)
        .addFunction("addChild", &QsTreeWidgetItem::addChild)
        .addFunction("setStringData", &QsTreeWidgetItem::setStringData)
        .addFunction("setIntData", &QsTreeWidgetItem::setIntData)
        .addFunction("stringData", &QsTreeWidgetItem::stringData)
        .addFunction("intData", &QsTreeWidgetItem::intData)
        .addFunction("setExpanded", &QsTreeWidgetItem::setExpanded)
        .addFunction("setTextColor", &QsTreeWidgetItem::setTextColor)
        .addFunction("empty", &QsTreeWidgetItem::empty)
    );
}

void QsTreeWidgetItem::setText(int column, const std::string& title)
{
    m_item->setText(column,STDtQ(title));
}

std::string QsTreeWidgetItem::text(int column)
{
    return QtSTD(m_item->text(column));
}

void QsTreeWidgetItem::addChild(QsTreeWidgetItem* item)
{
    m_item->addChild(item->item());
}

void QsTreeWidgetItem::setStringData(int column, int role, const std::string& data)
{
    m_item->setData(column, role, STDtQ(data));
}

void QsTreeWidgetItem::setIntData(int column, int role,int data)
{
    m_item->setData(column, role, data);
}

std::string QsTreeWidgetItem::stringData(int column, int role)
{
    return QtSTD(m_item->data(column, role).toString());
}

int QsTreeWidgetItem::intData(int column, int role)
{
    return m_item->data(column, role).toInt();
}

void QsTreeWidgetItem::setExpanded(bool expand)
{
    m_item->setExpanded(expand);
}

void QsTreeWidgetItem::setTextColor(int column, const std::string& hex_color_text)
{
    m_item->setTextColor(column,QColor(STDtQ(hex_color_text)));
}

