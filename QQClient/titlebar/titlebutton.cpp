#include "titlebutton.h"
#include <QDebug>

TitleButton::TitleButton(QWidget *parent,int tag,QString iconName)
    :QPushButton (parent),tag(tag)
{
    setFixedSize(40, 30);
    setIcon(QPixmap(iconName).scaled(15,15,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    setFlat(true);
    setStyleSheet("border:none");
}

TitleButton::~TitleButton()
{

}

void TitleButton::enterEvent(QEvent *)
{
    if(tag == 0){
        setFlat(false);
        setStyleSheet("QPushButton{background-color:rgb(232,17,35);border:none}");
        setIcon(QPixmap(":/Icons/MainWindow/close2.png").
                scaled(15,15,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    }else if(tag == 1){
        setFlat(false);
        setStyleSheet("QPushButton{background-color:rgb(200,200,200);border:none}");
    }
    setCursor(Qt::PointingHandCursor);
}

void TitleButton::leaveEvent(QEvent *e)
{

    setFlat(true);
    setStyleSheet("QPushButton{background-color:rgb(242,242,242);border:none}");
    setCursor(Qt::ArrowCursor);

    if(tag == 0)
        setIcon(QPixmap(":/Icons/MainWindow/close.png").
                scaled(15,15,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    return QPushButton::leaveEvent(e);
}
