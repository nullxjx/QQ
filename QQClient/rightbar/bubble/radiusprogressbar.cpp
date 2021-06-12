#include "radiusprogressbar.h"
#include <QPainter>
#include <QDebug>

RadiusProgressBar::RadiusProgressBar(QWidget *parent) : QProgressBar(parent)
{
    setMinimum(0);
    setMaximum(100);
    setValue(0);
}

void RadiusProgressBar::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect rect = QRect(0, 0, width(), height()/2);
    QRect textRect = QRect(0, height()/2, width(), height()/2);

    const double k = (double)(value() - minimum()) / (maximum()-minimum());
    //qDebug() << "sent: " << k << " value:" << value() << " min:" << minimum() << " max:" << maximum();

    int x = (int)(rect.width() * k);
    QRect fillRect = rect.adjusted(0, 0, x-rect.width(), 0);

    QString valueStr;
    if(tag == 0)
        valueStr = QString("已发送 %1%").arg(QString::number(value()));
    else if(tag == 1)
        valueStr = QString("已接收 %1%").arg(QString::number(value()));

    QPixmap buttomMap = QPixmap(":/Icons/MainWindow/radius_back.png");
    QPixmap fillMap = QPixmap(":/Icons/MainWindow/radius_front.png");

    //画进度条
    p.drawPixmap(rect, buttomMap);
    p.drawPixmap(fillRect, fillMap, fillRect);

    //画文字
    QFont f = QFont("Microsoft YaHei", 10, QFont::Bold);
    p.setFont(f);
    p.setPen(QColor("#555555"));
    p.drawText(textRect, Qt::AlignCenter, valueStr);
}

