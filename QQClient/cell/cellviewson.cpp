#include "cellviewson.h"
#include "global.h"
#include "database.h"

#include <QPainter>
#include <QPixmap>
#include <QDebug>

CellViewSon::CellViewSon(QWidget *parent,Cell *c,int tag)
    : QWidget(parent),cell(c),tag(tag)
{
    entered = false;

    if(cell->type == Cell_AddFriend || cell->type == Cell_AddGroup){
        notice = new QLabel(this);
        notice->setPixmap(QPixmap(":/Icons/MainWindow/notice.png").
                          scaled(40,40,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        notice->setFixedSize(40,40);
        notice->setStyleSheet("border:none;");
    }else{
        newMsg = new QLabel(this);
        QPixmap pixmap_(":/Icons/MainWindow/newmsg.png");
        newMsg->setPixmap(pixmap_.scaled(15,15,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        newMsg->setFixedSize(15,15);
        newMsg->setStyleSheet("border:none;");
        newMsg->setVisible(false);

        headLabel = new RoundLabel(this,cell->iconPath);
        if(tag == 0 || tag == 2) {
            headLabel->setFixedSize(40,40);
            if(tag == 0){
                if(cell->stayOnTop){
                    topLabel = new QLabel(this);
                    topLabel->setPixmap(QPixmap(":/Icons/MainWindow/top_chat.png").
                                        scaled(25,25,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
                }
            }
        } else if (tag == 1){
            headLabel->setFixedSize(30,30);

            isAdmin = DataBase::Instance()->isAdmin(cell->id,cell->groupid);
            if(isAdmin){
                adminLabel = new QLabel(this);
                adminLabel->setPixmap(QPixmap(":/Icons/MainWindow/admin.png").
                                      scaled(15,15,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
            }
        }
    }
}

void CellViewSon::setPopMenu(QMenu *menu)
{
    popMenu = menu;
}

void CellViewSon::mousePressEvent(QMouseEvent *e)
{
    if(this->cell == nullptr)
        return;

    if(e->button() == Qt::LeftButton){
        qDebug() << "LeftBtn clicked on viewson:" << cell->id << cell->name;
        cell->isClicked = true;
        cell->showNewMsg = false;
        update();

        emit onSelected(this->cell);
    }else if(e->button() == Qt::RightButton){

        if(popMenu == nullptr)
            return;
        qDebug() << "RightBtn clicked on viewson:" << cell->id << cell->name;
        emit onRightClicked(this->cell);
        emit onPopMenuToShow(cell,popMenu);

        if(cell->type == Cell_FriendContact && tag == 2){
            popMenu->clear();

            popMenu->addAction(tr("发消息"));
            popMenu->addSeparator();
            popMenu->addAction(tr("移动至黑名单"));
            popMenu->addAction(tr("删除联系人"));
            QMenu *sonGroupListMenu = new QMenu(tr("移动联系人至"));
            popMenu->addMenu(sonGroupListMenu);


            //查询本地数据库获取我的好友分组
            QJsonArray mySubgroup = DataBase::Instance()->getMySubgroup();
            for(int i = 0; i < mySubgroup.size(); i++){
                QJsonObject json = mySubgroup.at(i).toObject();
                sonGroupListMenu->addAction(json.value("name").toString());
            }
        }

        popMenu->exec(QCursor::pos());//在光标出显示右键菜单
    }

    //return QWidget::mousePressEvent(e);
}

void CellViewSon::mouseDoubleClickEvent(QMouseEvent *e)
{
    cell->isClicked = true;
    cell->showNewMsg = false;
    update();
    if(cell->type == Cell_AddFriend || cell->type == Cell_AddGroup)
        emit signalOpenDialog(this->cell);
    else
        emit onDoubleClicked(this->cell);

    return QWidget::mouseDoubleClickEvent(e);
}

void CellViewSon::enterEvent(QEvent *e)
{
    entered = true;
    update();

    return QWidget::enterEvent(e);
}

void CellViewSon::leaveEvent(QEvent *e)
{
    entered = false;
    update();

    return QWidget::leaveEvent(e);
}

void CellViewSon::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform,true);// 抗锯齿

    if(cell->isClicked){
        painter.fillRect(this->rect(),QColor("#D9D8D7"));
        if(tag == 0){
            painter.setBrush(QColor("#019cfe"));
            painter.drawRect(0,0,3,this->height());
        }
    } else {
        painter.fillRect(this->rect(),entered ? QColor("#D9D8D7") : QColor("transparent"));
    }

    //绘制头像
    if(tag == 0 || tag == 2){
        if(cell->type == Cell_AddFriend || cell->type == Cell_AddGroup)
            notice->setGeometry(10,10,40,40);
        else
            headLabel->setGeometry(10,10,40,40);
    }else if(tag == 1){
        headLabel->setGeometry(5,5,30,30);
        if(isAdmin)
            adminLabel->setGeometry(this->width()-30,this->height()/2 - 7,15,15);
    }

    int w = this->rect().width();
    int h = this->rect().height();
    QFont font("微软雅黑",14);
    QPen pen;

    if(tag == 0 || tag == 2){
        //绘制名字
        pen.setColor(Qt::black);
        font.setPixelSize(20);
        painter.setPen(pen);
        painter.setFont(font);

        if(cell->type == Cell_AddFriend){
            painter.drawText(70,0,w-70,h/2-2,Qt::AlignBottom,"新朋友");
        } else if (cell->type == Cell_AddGroup){
            painter.drawText(70,0,w-70,h/2-2,Qt::AlignBottom,"请求加群");
        }else{
            painter.drawText(70,0,w-70,h/2-2,Qt::AlignBottom,cell->name);
        }

        if(tag == 0){
            if(cell->type == Cell_AddFriend || cell->type == Cell_AddGroup){
                //绘制subtitle
                pen.setColor(QColor("#666666"));
                font.setPixelSize(15);
                painter.setPen(pen);
                painter.setFont(font);
                painter.drawText(w-100, 0, 100, h / 2 - 2, Qt::AlignCenter, cell->subTitle);
                painter.drawText(70, h/2 - 5, 260, h / 2 - 2, Qt::AlignBottom, cell->msg);
            }else{
                //绘制subtitle
                pen.setColor(QColor("#666666"));
                font.setPixelSize(15);
                painter.setPen(pen);
                painter.setFont(font);
                painter.drawText(w-100, 0, 100, h / 2 - 2, Qt::AlignCenter, cell->subTitle);
                painter.drawText(70, h/2 - 5, 260, h / 2 - 2, Qt::AlignBottom, cell->msg);

                if(!cell->isClicked && cell->showNewMsg){
                    newMsg->move(w-30,h / 2 - 5);
                    newMsg->setVisible(true);
                    //qDebug() << "show" << cell->subTitle;
                }else{
                    newMsg->setVisible(false);
                    newMsg->hide();
                }

                if(cell->stayOnTop){
                    topLabel->setGeometry(0,-5,25,25);
                }
            }
        }

    }else if(tag == 1){
        pen.setColor(Qt::black);
        font.setPixelSize(17);
        painter.setPen(pen);
        painter.setFont(font);
        painter.drawText(50,10,200,20,Qt::AlignBottom,cell->name);
    }

    //底下的分隔线
#if 0
    pen.setColor(QColor(Qt::white));
    painter.setPen(pen);
    if(tag == 0 || tag == 2) painter.drawLine(70,h-1,w,h-1);
    else if(tag == 1) painter.drawLine(50,h-1,w,h-1);
#endif

}

void CellViewSon::focusOutEvent(QFocusEvent *e)
{
    if(tag == 1 || tag == 2){
        cell->isClicked = false;
        update();
    }

    return QWidget::focusOutEvent(e);
}
