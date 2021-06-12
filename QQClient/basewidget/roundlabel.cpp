#include "roundlabel.h"

#include <QPixmap>
#include <QPainter>
#include <QDebug>

RoundLabel::RoundLabel(QWidget *parent,QString iconPath)
    : QLabel(parent)
{
    pixmap = QPixmap(iconPath);
    pixmap.scaled(this->width(),this->height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
}

void RoundLabel::setPixmap(QString iconpath)
{
    pixmap = QPixmap(iconpath);
    pixmap.scaled(this->width(),this->height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    update();
}

void RoundLabel::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform,true);// 抗锯齿

    QPainterPath path;
    int radius = this->width()/2;
    path.addEllipse(0,0,radius * 2,radius*2);
    painter.setClipPath(path);

    painter.drawPixmap(QRect(0,0,radius*2,radius*2),pixmap);

    return QLabel::paintEvent(e);
}
