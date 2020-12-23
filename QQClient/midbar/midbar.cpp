#include "midbar.h"
#include "titlebar.h"
#include "searchbar.h"
#include "mybutton.h"
#include "myapp.h"
#include "database.h"
#include "global.h"
#include "findfriendwnd.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPalette>
#include <QStackedWidget>
#include <QDebug>
#include <QFileInfo>
#include <QMenu>
#include <QSqlError>

MidBar::MidBar(QWidget *parent) : QWidget(parent)
{
    setWindowFlag(Qt::FramelessWindowHint);
    setFixedWidth(350);

    TitleBar *titleBar = new TitleBar(nullptr,1);
    titleBar->setStyleSheet("border: none;background-color:#ebeae8");

    SearchBar *searchBar = new SearchBar(nullptr,QSize(280,40));

    addMenu = new QMenu(this);
    addMenu->addAction(tr("添加好友"));
    addMenu->addAction(tr("添加群"));
    addMenu->addSeparator();
    addMenu->addAction(tr("创建群"));
    connect(addMenu,SIGNAL(triggered(QAction *)),
            this,SLOT(sltMenuSelected(QAction *)));

    QStringList list;
    list << ":/Icons/MainWindow/add.png"
         << ":/Icons/MainWindow/add.png"
         << ":/Icons/MainWindow/add.png";
    MyButton *addBtn = new MyButton(nullptr,list,QSize(30,30));
    connect(addBtn,&MyButton::clicked,[&](){
        addMenu->exec(QCursor::pos());
    });

    QHBoxLayout *search_layout = new QHBoxLayout;
    search_layout->addWidget(searchBar);
    search_layout->addSpacing(10);
    search_layout->addWidget(addBtn);
    search_layout->setContentsMargins(5,5,5,5);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(titleBar);
    layout->addLayout(search_layout);

    chatList = new ListWidget;
    connect(chatList,SIGNAL(popMenuToShow(Cell*,QMenu *)),
            this,SLOT(setPopMenuCell(Cell*,QMenu*)));
    connect(chatList,SIGNAL(signalOpenDialog(Cell*)),
            this,SLOT(sltAddFriend(Cell *)));

    contactWidget = new ContactWidget;
    connect(contactWidget,SIGNAL(signalSendMessage(const quint8 &, const QJsonValue &)),
            this,SIGNAL(signalSendMessage(const quint8 &, const QJsonValue &)));
    connect(contactWidget,SIGNAL(openDialog(Cell*)),this,SLOT(sltOpenDialog(Cell*)));
    connect(contactWidget,SIGNAL(deleteChat(int)),this,SLOT(deleteChatCell(int)));

    blankPage = new BlankPage(this);


    mainWidget = new QStackedWidget;
    mainWidget->addWidget(chatList);
    mainWidget->addWidget(contactWidget);
    mainWidget->addWidget(blankPage);
    mainWidget->setCurrentIndex(0);//默认打开chatList列表
    mainWidget->setContentsMargins(0,0,0,0);

    layout->addWidget(mainWidget);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);


    connect(chatList,&ListWidget::sonDoubleClicked,[&](Cell* cell){
        qDebug() << "打开聊天对话框：" << cell->name;
        selectedCell = cell;
        emit openDialog(cell);
    });

    //this->setStyleSheet("border: none;background-color:#ebeae8");
    QPalette palette;
    palette.setColor(QPalette::Background, QColor("#ebeae8"));
    setAutoFillBackground(true);  //一定要这句，否则不行
    setPalette(palette);

    newGroup = new CreateGroupWnd;
    connect(newGroup,SIGNAL(signalCreateGroup(const QJsonValue &)),
            this,SLOT(sltCreateGroup(const QJsonValue &)));

    newGroup->hide();
}

MidBar::~MidBar()
{
    QString sqlQuery = "update myFriend set tag = 0;";
    QString sqlQuery2 = "update myGroup set tag = 0;";
    QSqlQuery query;
    query.exec(sqlQuery);
    query.exec(sqlQuery2);

    QList<Cell*> cells = chatList->getAllCells();
    int cnt = cells.size();
    for(int i = 0;i < cnt;i++){
        if(cells.at(i)->type == Cell_FriendChat){
            QString sqlQuery = "update myFriend set tag = 1, lastMsg = '";
            sqlQuery.append(cells.at(i)->msg + "', lastTime = '");
            sqlQuery.append(cells.at(i)->subTitle + "'");
            sqlQuery.append(" where id=");
            sqlQuery.append(QString::number(cells.at(i)->id));

            query.exec(sqlQuery);
            //qDebug() << "sql error:" << query.lastError().text();
        }else if(cells.at(i)->type == Cell_GroupChat){
            QString sqlQuery = "update myGroup set tag = 1, lastMsg = '";
            sqlQuery.append(cells.at(i)->msg + "', lastTime = '");
            sqlQuery.append(cells.at(i)->subTitle + "'");
            sqlQuery.append(" where id=");
            sqlQuery.append(QString::number(cells.at(i)->id));

            query.exec(sqlQuery);
            //qDebug() << "sql error:" << query.lastError().text();
        }
    }
}

void MidBar::SltMainPageChanged(int page)
{
    if(page == 0){
        //qDebug() << "显示聊天列表" << endl;
        int cnt = chatList->getAllCells().size();
        if(cnt == 0){
            mainWidget->setCurrentIndex(2);
        }else{
            mainWidget->setCurrentIndex(0);
        }
    }else if(page == 1){
        //qDebug() << "显示联系人列表" << endl;
        mainWidget->setCurrentIndex(1);
    }else if(page == 2){
        //qDebug() << "显示设置界面" << endl;

    }
}

void MidBar::InitContactList()
{
    contactWidget->InitList();
}

void MidBar::InitChatList()
{

    //设置聊天列表中的格子右击菜单
    QMenu *sonMenu = new QMenu(this);
    sonMenu->addAction(tr("会话置顶"));
    sonMenu->addAction(tr("取消置顶"));
    sonMenu->addSeparator();
    sonMenu->addAction(tr("关闭会话"));
    sonMenu->addAction(tr("关闭全部会话"));
    connect(sonMenu,SIGNAL(triggered(QAction*)),
            this,SLOT(onSonMenuSelected(QAction*)));
    chatList->setSonPopMenu(sonMenu);

    //查询本地数据库获取我的好友
    QJsonArray myChatList = DataBase::Instance()->getMyChatList();
    int cnt = myChatList.size();
    if(cnt == 0){
        mainWidget->setCurrentIndex(2);
        return;
    }

    for(int i = 0;i < cnt;i++){
        QJsonObject json = myChatList.at(i).toObject();
        Cell *c = new Cell;
        c->id = json.value("id").toInt();
        c->name = json.value("name").toString();
        c->iconPath = json.value("head").toString();
        c->msg = json.value("lastMsg").toString();
        c->subTitle = json.value("lastTime").toString();
        int tag = json.value("tag").toInt();
        if(tag == 0){
            c->type = Cell_FriendChat;
            c->groupName = json.value("subgroup").toString();
        }else if(tag == 1){
            c->type = Cell_GroupChat;
        }

        chatList->insertCell(c);
    }

    chatList->refreshList();
}

Cell* MidBar::isIDExist(int id)
{
    QList<Cell*> cells = chatList->getAllCells();
    for(int i = 0;i < cells.size();i++){
        if(cells[i]->id == id && (cells[i]->type == Cell_FriendChat ||
                                  cells[i]->type == Cell_GroupChat))
            return cells[i];
    }
    return nullptr;
}

void MidBar::insertCell(Cell *c)
{
    QList<Cell*> cells = chatList->getAllCells();
    int cnt = cells.size();
    if(c->type == Cell_AddFriend){
        for(int i = 0;i < cnt;i++){
            if(cells.at(i)->id == c->id && (cells.at(i)->type == Cell_AddFriend ||
                                            cells.at(i)->type == Cell_AddGroup)){
                cells.at(i)->done = false;
                return;
            }
        }

        chatList->insertCell(c);
    }else if(c->type == Cell_AddGroup){
        for(int i = 0;i < cnt;i++){
            if(cells.at(i)->id == c->id && cells.at(i)->groupid == c->groupid &&
                    (cells.at(i)->type == Cell_AddFriend || cells.at(i)->type == Cell_AddGroup)){
                cells.at(i)->done = false;
                return;
            }
        }

        chatList->insertCell(c);
    }else if(c->type == Cell_FriendChat || c->type == Cell_GroupChat){
        for(int i = 0;i < cnt;i++){
            if(cells.at(i)->id == c->id && (cells.at(i)->type == Cell_FriendChat ||
                                            cells.at(i)->type == Cell_GroupChat)){
                return;
            }
        }

        chatList->insertCell(c);
    }
}

void MidBar::switchToChatList()
{
    mainWidget->setCurrentIndex(0);//中栏切换到聊天列表
}

void MidBar::addCellToContact(Cell *c)
{
    contactWidget->addCell(c);
}

void MidBar::deleteFriend(int id)
{
    QList<Cell*> cells = chatList->getAllCells();
    int cnt = cells.size();
    for(int i = 0;i < cnt;i++){
        if(cells.at(i)->id == id){
            cells.at(i)->deleted = true;//标记为已删除，下次用户再点击时弹出对话框提示
            contactWidget->deleteFriend(id);//更新联系人列表和本地数据库
            return;
        }
    }
}

void MidBar::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void MidBar::setPopMenuCell(Cell *cell, QMenu *)
{
    qDebug() << "popMenu show on cell:" << cell->id << cell->name;
    popMenuCell = cell;
}

void MidBar::updateTime(int id, qint64 time,QString msg)
{
    qDebug() << id << "聊天窗口收到新消息"
             << "time:" << QDateTime::fromSecsSinceEpoch(time).toString("hh:mm:ss")
             << "msg:" << msg
             << "更新中栏相应的聊天格子..." << endl;

    chatList->refreshCellTime(id,time,msg);
}

void MidBar::sltMenuSelected(QAction *action)
{
    qDebug() << action->text();
    if(!action->text().compare(tr("添加好友"))){
        FindFriendWnd *w = new FindFriendWnd(0);

        connect(w,SIGNAL(signalFind(const QJsonValue&)),this,
                SLOT(sltFind(const QJsonValue &)));
        connect(this,SIGNAL(signalFindFriendReply(const QJsonValue &)),
                w,SLOT(sltFindFriendReply(const QJsonValue &)));
        connect(w,SIGNAL(signalSendMessage(const quint8 &, const QJsonValue &)),
                this,SIGNAL(signalSendMessage(const quint8 &, const QJsonValue &)));

        w->exec();
    }else if(!action->text().compare(tr("添加群"))){
        FindFriendWnd *w = new FindFriendWnd(1);

        connect(w,SIGNAL(signalFind(const QJsonValue&)),this,
                SLOT(sltFind(const QJsonValue &)));
        connect(this,SIGNAL(signalFindFriendReply(const QJsonValue &)),
                w,SLOT(sltFindFriendReply(const QJsonValue &)));
        connect(w,SIGNAL(signalSendMessage(const quint8 &, const QJsonValue &)),
                this,SIGNAL(signalSendMessage(const quint8 &, const QJsonValue &)));

        w->exec();
    }else if(!action->text().compare(tr("创建群"))){
        newGroup->exec();
    }
}

void MidBar::sltFind(const QJsonValue &json)
{
    emit signalSendMessage(FindFriend,json);
}

void MidBar::sltCreateGroup(const QJsonValue &json)
{
    emit signalSendMessage(CreateGroup,json);
}

void MidBar::sltAddFriend(Cell *cell)
{
    emit resetRightPage();

    AddFriendWnd *w = new AddFriendWnd(cell);
    connect(w,SIGNAL(signalAddFriend(const quint8 &,const QJsonValue&)),
            this,SIGNAL(signalSendMessage(const quint8 &, const QJsonValue &)));
    connect(w,SIGNAL(signalAddChat(Cell*)),this,SIGNAL(signalAddChat(Cell*)));

    w->exec();
}

void MidBar::deleteChatCell(int id)
{
    QList<Cell*> cells = chatList->getAllCells();
    int cnt = cells.size();
    for(int i = 0;i < cnt;i++){
        if(cells.at(i)->id == id && (cells.at(i)->type == Cell_FriendChat
                                     || cells.at(i)->type == Cell_GroupChat)){
            //移除中栏中与该联系人的聊天格子
            if(cnt == 1){
                emit openDialog(nullptr);
            }else{
                if(i != cnt-1){//不是最后一个格子，关闭该格子后打开下一个
                    if(selectedCell != nullptr){
                        chatList->resetCellState();
                        cells.at(i+1)->isClicked = true;
                        chatList->refreshList();

                        //打开聊天对话框
                        if(cells.at(i+1)->type != Cell_AddFriend &&
                                cells.at(i+1)->type != Cell_AddGroup){
                            emit openDialog(cells.at(i+1));
                        }else{
                            emit resetRightPage();
                        }

                        //保持在联系人界面
                        emit contactBtnClicked();
                        mainWidget->setCurrentIndex(1);
                    }else{
                        emit resetRightPage();
                    }
                }else{//是最后一个格子，关闭该格子后打开上一个
                    if(selectedCell != nullptr){
                        chatList->resetCellState();
                        cells.at(i-1)->isClicked = true;
                        chatList->refreshList();

                        //打开聊天对话框
                        if(cells.at(i-1)->type != Cell_AddFriend &&
                                cells.at(i-1)->type != Cell_AddGroup){
                            emit openDialog(cells.at(i-1));
                        }else{
                            emit resetRightPage();
                        }

                        //保持在联系人界面
                        emit contactBtnClicked();
                        mainWidget->setCurrentIndex(1);
                    }else{
                        emit resetRightPage();
                    }
                }
            }

            chatList->removeCell(cells.at(i));

            return;
        }
    }
}


void MidBar::onSonMenuSelected(QAction *action)
{
    if(!action->text().compare(tr("会话置顶"))){
        qDebug() << "会话置顶";
        chatList->setCellToTop(popMenuCell);
    }else if(!action->text().compare(tr("取消置顶"))){
        qDebug() << "取消置顶";
        chatList->cancelCellOnTop(popMenuCell);
    }else if(!action->text().compare(tr("关闭会话"))){
        qDebug() << "关闭会话";
        if(popMenuCell != nullptr){
            QList<Cell*> cells = chatList->getAllCells();
            int cnt = cells.size();
            if(cnt == 1){
                emit openDialog(nullptr);
            }else{
                for(int i = 0;i < cnt;i++){
                    if(cells.at(i) == popMenuCell){
                        if(i != cnt-1){//不是最后一个格子，关闭该格子后打开下一个
                            if(selectedCell != nullptr){
                                chatList->resetCellState();
                                cells.at(i+1)->isClicked = true;
                                chatList->refreshList();

                                //打开新的聊天对话框
                                if(cells.at(i+1)->type != Cell_AddFriend &&
                                        cells.at(i+1)->type != Cell_AddGroup){
                                    emit openDialog(cells.at(i+1));
                                }else{
                                    emit resetRightPage();
                                }
                            }else{
                                emit resetRightPage();
                            }
                        }else{//是最后一个格子，关闭该格子后打开上一个
                            if(selectedCell != nullptr){
                                chatList->resetCellState();
                                cells.at(i-1)->isClicked = true;
                                chatList->refreshList();

                                //打开新的聊天对话框
                                if(cells.at(i-1)->type != Cell_AddFriend &&
                                        cells.at(i-1)->type != Cell_AddGroup){
                                    emit openDialog(cells.at(i-1));
                                }else{
                                    emit resetRightPage();
                                }
                            }else{
                                emit resetRightPage();
                            }
                        }
                    }
                }
            }

            chatList->removeCell(popMenuCell);

            int cnt2 = chatList->getAllCells().size();
            if(cnt2 == 0){
                mainWidget->setCurrentIndex(2);
            }
        }
    }else if(!action->text().compare(tr("关闭全部会话"))){
        qDebug() << "关闭全部会话";
        chatList->removeAllCells();
        emit openDialog(nullptr);

        mainWidget->setCurrentIndex(2);
    }
}

void MidBar::sltOpenDialog(Cell *c)
{
    mainWidget->setCurrentIndex(0);//中栏切换到聊天列表
    chatList->resetCellState();

    //注意cell从联系人列表切换到聊天列表时需要转换cell的类型
    Cell *cell = isIDExist(c->id);
    if(cell == nullptr){//证明聊天列表中没有和该用户的聊天记录
        cell = new Cell;
        cell->id = c->id;
        cell->name = c->name;
        cell->iconPath = c->iconPath;
        if(c->type == Cell_FriendContact){
            cell->type = Cell_FriendChat;
        }else if(c->type == Cell_GroupContact){
            cell->type = Cell_GroupChat;
        }
        insertCell(cell);
    }
    cell->isClicked = true;

    chatList->refreshList();

    emit openDialog(cell);
}
