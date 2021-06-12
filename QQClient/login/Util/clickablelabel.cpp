/****************************************************************************
** QQ-Like 响应点击操作的 QLabel
****************************************************************************/
#include "clickablelabel.h"



ClickableLabel::ClickableLabel(QWidget *parent) : QLabel(parent)
{

}

void ClickableLabel::mousePressEvent(QMouseEvent *)
{
    emit clickSignal(this);
}

void ClickableLabel::enterEvent(QEvent *)
{
    this->setStyleSheet("font-size: 12px;font-family:Microsoft YaHei; color: #12b7f5");
    setCursor(Qt::PointingHandCursor);
}

void ClickableLabel::leaveEvent(QEvent *)
{
    this->setStyleSheet("font-size: 12px;font-family:Microsoft YaHei; color: #a6a6a6");
    setCursor(Qt::ArrowCursor);
}
