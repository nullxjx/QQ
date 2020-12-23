#include "mybutton.h"
#include "global.h"

#include <QPixmap>
#include <QBitmap>
#include <QDebug>
#include <QPainter>
#include <QPen>

MyButton::MyButton(QWidget *parent,QStringList iconsList,QSize Iconsize,BtnType type):
    QPushButton(parent)
{
    if(iconsList.size() < 3){
        qDebug() << "mybutton.cpp : fatal error!icon number is too few";
    }
    NormalIcon = iconsList.at(0);
    MoveInIcon = iconsList.at(1);
    ClickIcon = iconsList.at(2);
    btnSize = Iconsize;
    btnType = type;

    setFixedSize(btnSize);
    setImage(NormalIcon);
}

void MyButton::enterEvent(QEvent *e)
{
    if(btnType == NormalBtn){
        if(!tag){
            setImage(MoveInIcon);
            setCursor(Qt::PointingHandCursor);
        }
    }else{
        setCursor(Qt::PointingHandCursor);
        //setStyleSheet("border:2px solid white;border-radius:25px;");
    }

    return QPushButton::enterEvent(e);
}

void MyButton::leaveEvent(QEvent *e)
{
    if(btnType == NormalBtn){
        if(!tag){
            setImage(NormalIcon);
            setCursor(Qt::ArrowCursor);
        }
    }else{
        setCursor(Qt::ArrowCursor);
        //resize(btnSize);
    }

    return QPushButton::leaveEvent(e);
}

void MyButton::paintEvent(QPaintEvent *event)
{
    return QPushButton::paintEvent(event);
}


#if 0
void LeftBarButton::mousePressEvent(QMouseEvent *)
{
    setIcon(ClickIcon);
    qDebug() << "mouse press:" << this->size();
}

void LeftBarButton::mouseReleaseEvent(QMouseEvent *)
{
    setIcon(NormalIcon);
    qDebug() << "mouse release:" << this->size();
}

#endif

void MyButton::setImage(QString iconName)
{
    if(btnType == HeadBtn){
        QPixmap pixmap = myHelper::PixmapToRound(QPixmap(iconName),btnSize.width()/2);
//        QPixmap pixmap = myHelper::PixmapToRound(
//                    QPixmap("E:\\100001.png"),btnSize.width()/2);
        setIcon(QIcon(pixmap));
        setIconSize(size());
        setStyleSheet("border:none;");
    }else{
        setIcon(QIcon(iconName));
        setIconSize(size());
        setStyleSheet("border:none;");
    }
}

void MyButton::onBtnClicked()
{
    if(btnType == NormalBtn){
        setImage(ClickIcon);
        tag = true;
    }
}

void MyButton::restoreBtn()
{
    if(btnType == NormalBtn){
        setImage(NormalIcon);
        tag = false;
    }
}

void MyButton::changeIconSet(QStringList iconsList)
{
    NormalIcon = iconsList.at(0);
    MoveInIcon = iconsList.at(1);
    ClickIcon = iconsList.at(2);
}
