#include "yqt/util.h"
#include "QtCore/qfile.h"
#include "QtGui/qimage.h"
#include "QtCore/qbuffer.h"
QPixmap to_pixmap(QWidget* widget,const QString& filepath){
    if(filepath.isEmpty())
        return QPixmap();
    QPixmap pixmap(filepath);
    if(pixmap.width() != widget->width() || pixmap.height() !=widget->height())
        pixmap = pixmap.scaled(widget->width(),widget->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    return pixmap;
}
QPixmap to_pixmap(QWidget* widget,const ylib::buffer& data){
    if(data.empty())
        return QPixmap();
    QPixmap pixmap;
    pixmap.loadFromData((uchar*)data.data(), data.length());
    if(pixmap.width() != widget->width() || pixmap.height() !=widget->height())
        pixmap = pixmap.scaled(widget->width(),widget->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    return pixmap;
}
void ylib::qt::background(QLabel *obj, const QString &filepath)
{
    QPixmap map = to_pixmap(obj,filepath);
    if(!map.isNull())
        obj->setPixmap(map);
}

void ylib::qt::background(QLabel *obj, const buffer &data)
{
    QPixmap map = to_pixmap(obj,data);
    if(!map.isNull())
        obj->setPixmap(map);
}

void ylib::qt::background(QLabel* obj, const QImage& img)
{
    QImage newImg;
    if (img.width() != obj->width() || img.height() != obj->height())
        newImg = img.scaled(obj->width(), obj->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    else
        newImg = img;
    obj->setPixmap(QPixmap::fromImage(newImg));
}

void ylib::qt::background(QPushButton *obj, const buffer &data)
{
    QPixmap map = to_pixmap(obj,data);
    if(!map.isNull())
    {
        obj->setIcon(QIcon(map));
        obj->setIconSize(QSize(obj->width(), obj->height()));
        obj->setFlat(true);
    }


}

void ylib::qt::background(QPushButton *obj, const QString &filepath)
{
    QPixmap map = to_pixmap(obj,filepath);
    if(!map.isNull())
    {
        obj->setIcon(QIcon(map));
        obj->setIconSize(QSize(obj->width(), obj->height()));
        obj->setFlat(true);
    }
}

void ylib::qt::setqss(QWidget *obj, const QString &qss_filepath)
{
    QFile file(qss_filepath);
    file.open(QFile::ReadOnly);
    obj->setStyleSheet(file.readAll().data());
}

void ylib::qt::format_image_data(QWidget *widget, buffer &data)
{
    QPixmap map =  to_pixmap(widget,data);
    QImage image = map.toImage();
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    data.clear();
    data.append(byteArray.constData(),byteArray.size());
}

ylib::buffer ylib::qt::read(const QString &filepath)
{
    if (filepath.isEmpty())
        return ylib::buffer();
    QFile file(filepath);
    file.open(QIODevice::ReadOnly);
    auto data = file.readAll();
    ylib::buffer result;
    result.append(data.data(),data.length());
    return result;
}


void ylib::qt::setX(QWidget* widget, int x)
{
    QRect rect;
    rect.setX(x);
    rect.setY(widget->y());
    rect.setWidth(widget->width());
    rect.setHeight(widget->height());
    widget->setGeometry(rect);
}
void ylib::qt::setY(QWidget* widget, int y)
{
    QRect rect;
    rect.setX(widget->x());
    rect.setY(y);
    rect.setWidth(widget->width());
    rect.setHeight(widget->height());
    widget->setGeometry(rect);
}
void ylib::qt::setWidth(QWidget* widget, int width)
{
    QRect rect;
    rect.setX(widget->x());
    rect.setY(widget->y());
    rect.setWidth(width);
    rect.setHeight(widget->height());
    widget->setGeometry(rect);
}
void ylib::qt::setHeight(QWidget* widget, int height) 
{
    QRect rect;
    rect.setX(widget->x());
    rect.setY(widget->y());
    rect.setWidth(widget->width());
    rect.setHeight(height);
    widget->setGeometry(rect);
}

