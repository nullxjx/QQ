#include "cellviewdad.h"
#include <QHBoxLayout>
#include <QPixmap>
#include <QDebug>

CellViewDad::CellViewDad(QWidget *parent) : QWidget(parent)
{
    setWindowFlag(Qt::FramelessWindowHint);
    setFixedSize(QSize(325,30));

    QHBoxLayout *layout = new QHBoxLayout(this);
    iconLabel = new QLabel;
    iconLabel->setFixedSize(QSize(15,15));
    titleLabel = new QLabel;
    subTitleLabel = new QLabel;
    subTitleLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(subTitleLabel);
    layout->setContentsMargins(0,0,0,0);

    setFont(QFont("微软雅黑",10,5,false));
}

void CellViewDad::setCell(Cell *c)
{
    cell = c;
    if(cell->isOpen){
        iconLabel->setPixmap(QPixmap(":/Icons/MainWindow/arrowDown.png").
                             scaled(iconLabel->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    }else{
        iconLabel->setPixmap(QPixmap(":/Icons/MainWindow/arrowRight.png").
                             scaled(iconLabel->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    }
}

void CellViewDad::setPopMenu(QMenu *menu)
{
    popMenu = menu;
}

void CellViewDad::mousePressEvent(QMouseEvent *e)
{
    if(cell == nullptr) return;
    if(e->button() == Qt::LeftButton){//处理鼠标左键单击事件
        cell->isOpen = !cell->isOpen;//鼠标左键单击，改变cell的打开状态
        if(cell->isOpen) iconLabel->setPixmap(QPixmap(":/Icons/MainWindow/arrowDown.png").scaled(iconLabel->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        else iconLabel->setPixmap(QPixmap(":/Icons/MainWindow/arrowRight.png").scaled(iconLabel->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        qDebug() << "LeftBtn clicked on viewdad:" << cell->id << cell->name;

        emit onOpenStatusChanged(this);
    }else if(e->button() == Qt::RightButton){
        if(popMenu == nullptr) return;
        qDebug() << "RightBtn clicked on viewdad:" << cell->id << cell->name;
        emit onPopMenuToShow(cell,popMenu);
        popMenu->exec(QCursor::pos());//在光标位置弹出右键菜单
    }

    return QWidget::mousePressEvent(e);
}

