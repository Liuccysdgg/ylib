
#include "yqlua/QsTableWidget.h"
#include "yqlua/define.h"
#include "QtWidgets/qstyleditemdelegate.h"
#include "QtGui/qpainter.h"
class NoFocusDelegate : public QStyledItemDelegate
{
public:
    NoFocusDelegate() {}
    ~NoFocusDelegate() {}
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
        QStyleOptionViewItem itemOption(option);
        // 去掉选中时的虚线框
        if ((itemOption.state & QStyle::State_HasFocus))
        {
            itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
        }
        else
        {

        }
        // 设置选中的item，字体颜色和选中前的颜色一样
        itemOption.palette.setColor(QPalette::HighlightedText, index.data(Qt::ForegroundRole).value<QColor>());
        QStyledItemDelegate::paint(painter, itemOption, index);
    }
};


QsTableWidgetItem::QsTableWidgetItem(QTableWidgetItem* item,QsTableWidget* parent, bool create)
{
    if (item == nullptr)
    {
        if(create)
            m_item = new QTableWidgetItem(nullptr);
    }
    else
        m_item = item;

   
}

QsTableWidgetItem::~QsTableWidgetItem()
{
}

int QsTableWidgetItem::row()
{
    return m_item->row();
}
void QsTableWidgetItem::setIcon(QsIcon icon)
{
    m_item->setIcon(icon);
}
void QsTableWidgetItem::setColor(const std::string& hex)
{
    m_item->setForeground(QBrush(QColor(hex.c_str())));
}
void QsTableWidgetItem::regist(kaguya::State& T)
{
    T["QsTableWidgetItem"].setClass(kaguya::UserdataMetatable<QsTableWidgetItem>()
        .setConstructors<QsTableWidgetItem(QTableWidgetItem*,QsTableWidget*)>()
        .addFunction("setText", &QsTableWidgetItem::setText)
        .addFunction("row", &QsTableWidgetItem::row)
        .addFunction("empty", &QsTableWidgetItem::empty)
        .addFunction("setIcon", &QsTableWidgetItem::setIcon)
        .addFunction("setColor", &QsTableWidgetItem::setColor)
    );
}

void QsTableWidgetItem::setText(const std::string& text)
{
    m_item->setText(STDtQ(text));
}



QsTableWidget::QsTableWidget(QsWidget* parent):ylib::qt::TableWidget(parent->operator QWidget *())
{
    QObject::connect(this, &QTableWidget::itemClicked, [&](QTableWidgetItem* item) {
        if (m_onItemClicked.isNilref())
            return;
        m_onItemClicked(new QsTableWidgetItem(item));
    });
    setItemDelegate(new NoFocusDelegate()); //设置点击的表格无虚线

  //  initEvent();
    //viewport()->installEventFilter(this);
}

QsTableWidget::~QsTableWidget()
{
}

void QsTableWidget::regist(kaguya::State& L)
{

    L["QsTableWidget"].setClass(kaguya::UserdataMetatable<QsTableWidget>()
        .setConstructors<QsTableWidget(QsWidget*)>()
        .addFunction("toQ", &QsTableWidget::toQ)
        .addFunction("setColumnCount", &QsTableWidget::setColumnCount)
        .addFunction("setColumnWidth", &QsTableWidget::setColumnWidth)
        .addFunction("setSelectionBehavior", &QsTableWidget::setSelectionBehavior)
        .addFunction("setSelectionMode", &QsTableWidget::setSelectionMode)
        .addFunction("setEditTriggers", &QsTableWidget::setEditTriggers)
        .addFunction("horizontalHeader", &QsTableWidget::horizontalHeader)
        .addFunction("verticalHeader", &QsTableWidget::verticalHeader)
        .addFunction("setRowCount", &QsTableWidget::setRowCount)
        .addFunction("insertRow", &QsTableWidget::insertRow)
        .addFunction("setItem", &QsTableWidget::setItem)
        .addFunction("rowCount", &QsTableWidget::rowCount)
        .addFunction("onItemClicked", &QsTableWidget::onItemClicked)
        .addFunction("setHorizontalHeaderLabels", &QsTableWidget::setHorizontalHeaderLabels)
        .addFunction("currentItem", &QsTableWidget::currentItem)
        .addFunction("verticalScrollBar", &QsTableWidget::verticalScrollBar)
        .addFunction("horizontalScrollBar", &QsTableWidget::horizontalScrollBar)
        .addFunction("setCellWidget", &QsTableWidget::setCellWidget)
        .addFunction("setCurrentItem", &QsTableWidget::setCurrentItem)
        .addFunction("item", &QsTableWidget::item)
        .addFunction("itemClickedEvent", &QsTableWidget::itemClickedEvent)
        .addFunction("onLeave", &QsTableWidget::onLeave)
        .addFunction("onEnter", &QsTableWidget::onEnter)

 
    );
    QsTableWidgetItem::regist(L);
}

void QsTableWidget::setSelectionBehavior(int value)
{
    ylib::qt::TableWidget::setSelectionBehavior((QAbstractItemView::SelectionBehavior)value);
}

void QsTableWidget::setSelectionMode(int value)
{

    ylib::qt::TableWidget::setSelectionMode((QAbstractItemView::SelectionMode)value);
}

void QsTableWidget::setEditTriggers(int value)
{
    ylib::qt::TableWidget::setEditTriggers((EditTriggers)value);
}

void QsTableWidget::setItem(int row, int column, QsTableWidgetItem* item)
{
    ylib::qt::TableWidget::setItem(row,column,item->item());
}

void QsTableWidget::setHorizontalHeaderLabels(const kaguya::LuaTable& labels)
{
    std::vector<std::string> cppArray;

    // 将 Lua 数组中的元素转换为 C++ 数组
    QStringList qlabels;
    for (size_t i = 1; i <= labels.size(); ++i) {
        qlabels.push_back(STDtQ(labels[i].get<std::string>()));
    }
    ylib::qt::TableWidget::setHorizontalHeaderLabels(qlabels);
}
void QsTableWidget::setCellWidget(int row, int column, QsWidget* widget)
{
    ylib::qt::TableWidget::setCellWidget(row,column,widget->operator QWidget *());
}
QsHeaderView QsTableWidget::horizontalHeader()
{
    return QsHeaderView(ylib::qt::TableWidget::horizontalHeader());
}

QsHeaderView QsTableWidget::verticalHeader()
{
    return QsHeaderView(ylib::qt::TableWidget::verticalHeader());
}
QsScrollBar QsTableWidget::verticalScrollBar()
{
    return QsScrollBar(ylib::qt::TableWidget::verticalScrollBar());
}
QsScrollBar QsTableWidget::horizontalScrollBar()
{
    return QsScrollBar(ylib::qt::TableWidget::horizontalScrollBar());
}
void QsTableWidget::onItemClicked(kaguya::LuaFunction callback)
{
    m_onItemClicked = callback;
}

QsTableWidgetItem QsTableWidget::currentItem()
{
    return QsTableWidgetItem(QTableWidget::currentItem(),nullptr,false);
}
void QsTableWidget::setCurrentItem(QsTableWidgetItem item)
{
    QTableWidget::setCurrentItem(item.item());
}
QsTableWidgetItem QsTableWidget::item(int row, int column)
{
    return QsTableWidgetItem(QTableWidget::item(row, column));
}
//bool QsTableWidget::eventFilter(QObject* object, QEvent* event)
//{
//    if (object == this->viewport())
//    {
//        if (event->type() == QEvent::MouseMove)
//        {
//            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
//            QTableWidgetItem* currentItem = itemAt(mouseEvent->pos());
//
//            if (currentItem != m_lastItem)
//            {
//                if (m_lastItem)
//                {
//                    // 处理划出事件
//                    // ...
//                    if(!m_onLeave.isNilref())
//                        m_onLeave(QsTableWidgetItem(m_lastItem));
//                }
//
//                if (currentItem)
//                {
//                    // 处理划入事件
//                    // ...
//                    if (!m_onEnter.isNilref())
//                        m_onEnter(QsTableWidgetItem(m_lastItem));
//                }
//
//                m_lastItem = currentItem;
//            }
//        }
//    }
//    return QTableWidget::eventFilter(object, event);
//}