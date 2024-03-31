#pragma once
#include "QtWidgets/qlabel.h"
#include "QtWidgets/qpushbutton.h"

#include "ybase/buffer.h"

namespace ylib::qt
{ 
    void background(QLabel* obj, const QString& filepath);
    void background(QLabel* obj, const ylib::buffer& data);
    void background(QLabel* obj, const QImage& img);
    void background(QPushButton* obj, const ylib::buffer& data);
    void background(QPushButton* obj, const QString& filepath);
    void setqss(QWidget* obj,const QString& qss_filepath);

    /**
     * @brief 优化图像大小，根据对象尺寸
     * @param widget
     * @param data
     */
    void format_image_data(QWidget* widget,ylib::buffer& data);

    ylib::buffer read(const QString& filepath);


    void setX(QWidget* widget,int x);
    void setY(QWidget* widget, int y);
    void setWidth(QWidget* widget, int width);
    void setHeight(QWidget* widget, int height);
}


