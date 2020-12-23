#include "listwidget.h"
#include "global.h"

#include <QDebug>
#include <QPalette>
#include <QHBoxLayout>

ListWidget::ListWidget(QWidget *parent,int tag) :
    QListWidget(parent),tag(tag)
{
    rightClickedCell = nullptr;

    this->setVerticalScrollMode(QListWidget::ScrollPerPixel);
    //    this->verticalScrollBar()->setSingleStep(10);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //tag == 0表示聊天列表
    //tag == 1表示群成员列表
    //tag == 2表示联系人列表
    if(tag == 0 || tag == 2) this->setStyleSheet("ListWidget{background:#ebeae8;border:none;}");//对话列表的颜色
    else if(tag == 1) this->setStyleSheet("ListWidget{background:#f0f0f0;border:none;}");//群成员列表的颜色,f0f0f0


    scrollBar = new FloatingScrollBar(this,Qt::Vertical);
    connect(this->verticalScrollBar(),SIGNAL(valueChanged(int)),
            scrollBar,SLOT(slt_valueChanged(int)));
    connect(scrollBar,SIGNAL(valueChanged(int)),this,SLOT(slt_valueChanged(int)));
    connect(this->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),
            scrollBar,SLOT(slt_rangeChanged(int,int)));
}

Cell* ListWidget::getRightClickedCell()
{
    return rightClickedCell;
}

void ListWidget::setDadPopMenu(QMenu *menu)
{
    cellDadMenu = menu;
}

void ListWidget::setSonPopMenu(QMenu *menu)
{
    cellSonMenu = menu;
}

void ListWidget::setCellToTop(Cell *cell)
{
    cell->stayOnTop = true;
    cells.removeOne(cell);
    cells.push_front(cell);
    refreshList();
}

void ListWidget::cancelCellOnTop(Cell *cell)
{
    cell->stayOnTop = false;
    cells.removeOne(cell);
    cells.push_back(cell);
    refreshList();
}

void ListWidget::insertCell(Cell *cell)
{
    if(cell->type == Cell_GroupDrawer || cell->type == Cell_FriendDrawer){
        cells.append(cell);
    }else if(cell->type == Cell_FriendContact || cell->type == Cell_GroupContact){
        if(tag == 2){
            foreach(Cell *group,cells){
                if(!group->groupName.compare(cell->groupName)){
                    group->childs.append(cell);
                    break;
                }
            }
        }else if(tag == 1){
            cells.append(cell);
        }

    }else if(cell->type == Cell_FriendChat || cell->type == Cell_GroupChat){
        cells.append(cell);
    }else if(cell->type == Cell_AddFriend || cell->type == Cell_AddGroup){
        cells.append(cell);
    }

    refreshList();
}

void ListWidget::removeCell(Cell *cell)
{
    if(cell->type == Cell_GroupDrawer || cell->type == Cell_FriendDrawer){
        cells.removeOne(cell);
    }else if(cell->type == Cell_FriendChat || cell->type == Cell_GroupChat){
        cells.removeOne(cell);
    }else if(cell->type == Cell_AddFriend || cell->type == Cell_AddGroup){
        cells.removeOne(cell);
    }else if(cell->type == Cell_FriendContact || cell->type == Cell_GroupContact){
        if(tag == 2){
            foreach(Cell *group,cells){
                if(!group->groupName.compare(cell->groupName)){
                    group->childs.removeOne(cell);
                    break;
                }
            }
        }else if(tag == 1){
            cells.removeOne(cell);
        }
    }
    refreshList();
}

void ListWidget::removeAllCells()
{
    if(tag == 0){
        cells.clear();
        refreshList();
    }
}


void ListWidget::refreshList()
{
    this->clear();//首先移除所有格子
    sonItems.clear();
    for(Cell *cell : cells){
        if(cell->type == Cell_GroupDrawer || cell->type == Cell_FriendDrawer) {

            CellViewDad *group = new CellViewDad();
            group->setGeometry(0,0,310,30);
            if(cell->type == Cell_FriendDrawer){
                int onLineCnt = 0;
                //统计在线好友数
                for(Cell *child : cell->childs){
                    if(child->status == OnLine){
                        onLineCnt++;
                    }
                }
                group->titleLabel->setText(cell->groupName);
                group->subTitleLabel->setText(QString("[%1/%2]").arg(onLineCnt).arg(cell->childs.size()));
            }else{
                group->titleLabel->setText(QString("%1[%2/%2]").arg(cell->groupName).arg(cell->childs.size()));
                group->subTitleLabel->setText("");
            }

            group->setCell(cell);//cell中封装的是数据，CellView负责显示，此处是把数据传递给界面显示
            group->setPopMenu(cellDadMenu);

            connect(group,SIGNAL(onOpenStatusChanged(CellViewDad *)),
                    this,SLOT(onDadOpenChanged(CellViewDad *)));
            connect(group,SIGNAL(onPopMenuToShow(Cell *, QMenu *)),
                    this,SIGNAL(popMenuToShow(Cell *, QMenu *)));

            QListWidgetItem *item = new QListWidgetItem("");
            item->setBackgroundColor(QColor(235, 234, 232));
            this->addItem(item);
            this->setItemWidget(item,group);
            item->setSizeHint(group->geometry().size());

            //如果抽屉被打开的话则显示下面的格子
            if(cell->isOpen){
                //先加载在线好友
                for(Cell *c : cell->childs){
                    if(c->status == OnLine)
                        addSonItem(c);
                }

                //再加载离线好友
                for(Cell *c : cell->childs){
                    if(c->status == OffLine)
                        addSonItem(c);
                }
            }
        }
        else{
            addSonItem(cell);
        }
    }
}

void ListWidget::addSonItem(Cell *cell)
{  
    CellViewSon *son = new CellViewSon(nullptr,cell,tag);

    if(tag == 0 || tag == 2)
        son->setGeometry(0,0,350,60);
    else if(tag == 1)
        son->setGeometry(0,0,200,40);
    son->setPopMenu(cellSonMenu);
    sonItems.append(son);

    //槽连接，消息传递给上层类进行具体处理


    connect(son,SIGNAL(onSelected(Cell *)),
            this,SLOT(onSonSelected(Cell *)));//单元格被单击选中
    connect(son,SIGNAL(onRightClicked(Cell *)),
            this,SLOT(onCellRightClicked(Cell *)));
    connect(son,SIGNAL(onPopMenuToShow(Cell *, QMenu *)),
            this,SIGNAL(popMenuToShow(Cell *, QMenu *)));
    connect(son,SIGNAL(onDoubleClicked(Cell *)),
            this,SIGNAL(sonDoubleClicked(Cell *)));

    QListWidgetItem *item = new QListWidgetItem("");
    this->addItem(item);
    this->setItemWidget(item,son);
    item->setSizeHint(son->geometry().size());

}

void ListWidget::changeSonSelectionState(Cell *c)
{
    for(CellViewSon* son: sonItems){
        if(son->cell != c){
            son->cell->isClicked = false;
            son->update();
        }
    }
}

void ListWidget::onDadOpenChanged(CellViewDad* dad)
{
    int cnt = dad->cell->childs.size();
    for(int i = 0;i < cnt;i++){
        dad->cell->childs.at(i)->isClicked = false;
    }
    refreshList();
}

void ListWidget::onSonSelected(Cell *cell)
{
    cell->isClicked = true;//选中该项
    changeSonSelectionState(cell);

    if(cell->type == Cell_AddFriend || cell->type == Cell_AddGroup)
        emit signalOpenDialog(cell);
    else if(cell->type == Cell_FriendChat || cell->type == Cell_GroupChat)
        emit sonDoubleClicked(cell);
}

void ListWidget::slt_valueChanged(int value)
{
    this->verticalScrollBar()->setValue(value);
}

Cell* ListWidget::getDadCellFromName(QString *name)
{
    for(Cell *group : cells){
        if(!group->groupName.compare(name))
            return group;
    }
    return nullptr;
}

void ListWidget::onCellRightClicked(Cell *cell)
{
    rightClickedCell = cell;
    emit signalSonRightClicked();
}

QList<Cell*> ListWidget::getAllCells() const
{
    return cells;
}

void ListWidget::resetCellState()
{
    int cnt = cells.size();
    for(int i = 0;i < cnt;i++){
        cells.at(i)->isClicked = false;
    }
}

void ListWidget::refreshCellTime(int id, qint64 time,QString msg)
{
    int cnt = cells.size();
    for(int i = 0;i < cnt;i++){
        if(cells.at(i)->id == id && (cells.at(i)->type == Cell_FriendChat
                                     || cells.at(i)->type == Cell_GroupChat)){
            cells.at(i)->subTitle = QDateTime::fromSecsSinceEpoch(time).toString("hh:mm:ss");

            msg.replace("\n"," ");
            //qDebug() << "更新中栏格子" << msg;

            cells.at(i)->msg = msg;

            if(!cells.at(i)->isClicked)
                cells.at(i)->showNewMsg = true;
            refreshList();
            return;
        }
    }
}


void ListWidget::enterEvent(QEvent *e)
{
    if(scrollBar->maximum() > 0)
        scrollBar->show();
    return QListWidget::enterEvent(e);
}

void ListWidget::leaveEvent(QEvent *e)
{
    scrollBar->hide();
    return QListWidget::leaveEvent(e);
}

void ListWidget::resizeEvent(QResizeEvent *e)
{
    int x = this->width() - 8;
    scrollBar->setGeometry(x,1,8,this->height()-2);
    return QListWidget::resizeEvent(e);
}

//-----------------FloatingScrollBar----------------------

FloatingScrollBar::FloatingScrollBar(QWidget *parent, Qt::Orientation t)
    :QScrollBar (parent)
{
    this->setOrientation(t);

    QString style_file = ":/qss/scrollbar.qss";
    QFile styleFile(style_file);//路径名
    if(styleFile.open( QFile::ReadOnly )){
        QString style( styleFile.readAll() );
        this->setStyleSheet(style);
    }

    this->setRange(0,0);
    this->hide();
}

void FloatingScrollBar::slt_valueChanged(int value)
{
    this->setValue(value);
}

void FloatingScrollBar::slt_rangeChanged(int min,int max)
{
    this->setMinimum(min);
    this->setRange(0,max);
    this->setPageStep(int(0.75*(this->height())) + max);
    if(max <= 0)
        this->hide();
}
