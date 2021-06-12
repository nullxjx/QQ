#include "rightbar.h"
#include "global.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QIcon>
#include <thread>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <windowsx.h>
#endif

RightBar::RightBar(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlag(Qt::FramelessWindowHint);

    titleBar = new TitleBar(this);
    connect(titleBar,SIGNAL(stayOnTop(bool)),this,SIGNAL(stayOnTop(bool)));

    stackWidget = new QStackedWidget;
    defaultPage = new DefaultPage;
    stackWidget->addWidget(defaultPage);
    stackWidget->setCurrentIndex(0);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(titleBar);
    mainLayout->addWidget(stackWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 1, 0);
    setLayout(mainLayout);

    loadingMovie = new QMovie(this);
    loadingMovie->setFileName(":/Icons/MainWindow/loading2.gif");

    loadingLabel = new QLabel(this);
    loadingLabel->setMovie(loadingMovie);
    loadingLabel->setFixedSize(50,50);
    loadingLabel->setAttribute(Qt::WA_TranslucentBackground,true);
    loadingLabel->setAutoFillBackground(false);
    loadingMovie->start();
    loadingLabel->setVisible(false);
}

void RightBar::switchPage(Cell *c)
{
    if(c == nullptr){
        resetPage();
        return;
    }

    if(hash.contains(c->id)){
        currentPage = hash[c->id];
        stackWidget->setCurrentIndex(currentPage);
    }else{
        ChatWindow *newWindow = new ChatWindow(nullptr,c);

        //ChatWindow的消息借用RightBar中转，最后传递给MainUI，因为发送消息的tcpsocket在MainUI类中
        connect(newWindow,SIGNAL(signalSendMessage(const quint8 &, const QJsonValue &)),
                this,SIGNAL(signalSendMessage(const quint8 &, const QJsonValue &)));
        connect(newWindow,SIGNAL(updateMidBarTime(int,qint64,QString)),
                this,SIGNAL(updateMidBarTime(int,qint64,QString)));

        if(c->type == Cell_FriendChat)
            newWindow->setTag(0);//标记为私聊窗口
        else if(c->type == Cell_GroupChat){
            newWindow->setTag(1);//标记为群聊窗口
            newWindow->InitGroupList();
        }

        chatWindowList.append(newWindow);
        hash.insert(c->id,cnt);

        stackWidget->addWidget(newWindow);
        currentPage = cnt;
        stackWidget->setCurrentIndex(currentPage);
        cnt++;

        this->startLoadingAnimation();
        newWindow->loadMsgFromDatabase();
        this->stopLoadingAnimation();
    }
}

void RightBar::msgconfirmed(QJsonValue dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();
        int reveiverID = json.value("to").toInt();
        qint64 time = json.value("time").toInt();
        for(int i = 0;i < chatWindowList.size();i++){
            if(chatWindowList.at(i)->getID() == reveiverID){//找到了该条消息对应的聊天窗口
                //消除动画
                chatWindowList.at(i)->msgWindow->msgConfirmed(time);
                return;
            }
        }
    }
}

void RightBar::msgReceived(Cell*c, QJsonValue dataVal)
{
    for(int i = 0;i < chatWindowList.size();i++){
        if(c->id == chatWindowList.at(i)->getID()){
            chatWindowList.at(i)->msgReceived(dataVal);//聊天窗口添加一条消息
            return;
        }
    }

    //证明没有打开与该用户的聊天框
    ChatWindow *newWindow = new ChatWindow(nullptr,c);

    //ChatWindow的消息借用RightBar中转，最后传递给MainUI，因为发送消息的tcpsocket在MainUI类中
    connect(newWindow,SIGNAL(signalSendMessage(const quint8 &, const QJsonValue &)),
            this,SIGNAL(signalSendMessage(const quint8 &, const QJsonValue &)));
    connect(newWindow,SIGNAL(updateMidBarTime(int,qint64,QString)),
            this,SIGNAL(updateMidBarTime(int,qint64,QString)));

    if(c->type == Cell_FriendChat)
        newWindow->setTag(0);
    else if(c->type == Cell_GroupChat){
        newWindow->setTag(1);
        newWindow->InitGroupList();
    }

    chatWindowList.append(newWindow);
    hash.insert(c->id,cnt);
    stackWidget->addWidget(newWindow);
    stackWidget->setCurrentIndex(currentPage);
    cnt++;

    this->startLoadingAnimation();
    newWindow->loadMsgFromDatabase();
    this->stopLoadingAnimation();

    newWindow->msgReceived(dataVal);//聊天窗口添加一条消息
}

void RightBar::refreshGroupList(QJsonValue dataVal)
{
    if (dataVal.isArray()) {
        QJsonArray array = dataVal.toArray();
        int cnt = array.size();
        if(cnt > 0){
            QJsonObject json = array.at(0).toObject();//提取群号
            int groupid = json.value("groupid").toInt();

            for(int i = 0;i < chatWindowList.size();i++){
                if(groupid == chatWindowList.at(i)->getID()){
                    chatWindowList.at(i)->refreshGroupList(dataVal);//刷新聊天窗口的群列表
                    qDebug() << "群聊窗口" << groupid << "正在刷新群员列表" << endl;
                    return;
                }
            }
        }
    }
}

void RightBar::addNewUserToGroupList(int groupID, Cell *newUser)
{
    int cnt = chatWindowList.size();
    for(int i = 0;i < cnt;i++){
        if(groupID == chatWindowList.at(i)->getID()){
            chatWindowList.at(i)->addCellToGroupList(newUser);
            return;
        }
    }
}

void RightBar::removeUserFromGroupList(int groupID, int userID)
{
    int cnt = chatWindowList.size();
    for(int i = 0;i < cnt;i++){
        if(groupID == chatWindowList.at(i)->getID()){
            //注意，若右栏没有对应的群聊天窗口，则不处理这条消息
            chatWindowList.at(i)->removeCellFromGroupList(userID);
            return;
        }
    }
}

void RightBar::resetPage()
{
    stackWidget->setCurrentIndex(0);
}

void RightBar::restorePage()
{
    stackWidget->setCurrentIndex(currentPage);
}

void RightBar::forbidSendMsg(int id)
{
    int cnt = chatWindowList.size();
    for(int i = 0;i < cnt;i++){
        if(id == chatWindowList.at(i)->getID()){
            chatWindowList.at(i)->forbidSendMsg();
            return;
        }
    }
}

void RightBar::allowSendMsg(int id)
{
    int cnt = chatWindowList.size();
    for(int i = 0;i < cnt;i++){
        if(id == chatWindowList.at(i)->getID()){
            chatWindowList.at(i)->allowSendMsg();
            return;
        }
    }
}

void RightBar::startLoadingAnimation()
{
    loadingLabel->setVisible(true);
    loadingLabel->setGeometry(this->width()/2 - 25,this->height()/2 - 100,50,50);
}

void RightBar::stopLoadingAnimation()
{
    loadingMovie->stop();
    loadingLabel->setVisible(false);
}

RightBar::~RightBar()
{

}
