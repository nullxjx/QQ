#include "mainui.h"
#include "myapp.h"
#include "database.h"
#include "global.h"
#include "windwmapi.h"
#include "aero.h"

#include <QHBoxLayout>
#include <QDebug>
#include <QFileInfo>

#pragma comment(lib, "user32.lib")

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <windowsx.h>
#endif

MainUI::MainUI(QWidget *parent) : QWidget(parent)
{
    setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    setWindowOpacity(0.95);
    setMinimumHeight(600);
    resize(1300,850);
    setWindowIcon(QIcon(":/Icons/MainWindow/title.png"));

    QHBoxLayout *layout = new QHBoxLayout(this);
    leftBar = new LeftBar;
    midBar = new MidBar;
    rightBar = new RightBar;

    layout->addWidget(leftBar);
    layout->addWidget(midBar);
    layout->addWidget(rightBar);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    connect(rightBar,SIGNAL(stayOnTop(bool)),this,SLOT(stayOnTop(bool)));
    connect(rightBar,SIGNAL(updateMidBarTime(int,qint64,QString)),
            midBar,SLOT(updateTime(int,qint64,QString)));
    connect(leftBar->m_btnGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(onBtnClicked(int)));

    connect(midBar,&MidBar::openDialog,[&](Cell *cell){
        if(cell == nullptr){
            rightBar->resetPage();
            rightBar->setCurrentPage(0);
        }else{
            qDebug() << cell->id << "is opening..." << endl;
            rightBar->switchPage(cell); //点击中栏的聊天对话列表，切换右栏中的聊天界面
            onBtnClicked(0);
        }
    });

    connect(midBar,&MidBar::resetRightPage,[&](){
        rightBar->resetPage();
        rightBar->setCurrentPage(0);
    });

    connect(midBar,&MidBar::contactBtnClicked,[&](){
        onBtnClicked(1);
    });


    settings = new SettingsWnd;
    settings->setVisible(false);

    //for test
    //rightBar->setTitleText("Me: " + QString::number(MyApp::m_nId) + "(" + MyApp::m_strUserName + ")");

    //绘制窗口边缘阴影
    HWND hwnd = HWND(this->winId());
    DWORD style = DWORD(::GetWindowLong(hwnd, GWL_STYLE));

    // 此行代码可以带回Aero效果，同时也带回了标题栏和边框,在nativeEvent()会再次去掉标题栏
    ::SetWindowLong(hwnd, GWL_STYLE, long(style) | long(WS_MAXIMIZEBOX) | long(WS_THICKFRAME) | long(WS_CAPTION));

    //we better left 1 piexl width of border untouch, so OS can draw nice shadow around it
    const MARGINS shadow = { 1, 1, 1, 1 };
    WinDwmapi::instance()->DwmExtendFrameIntoClientArea(HWND(winId()), &shadow);

    //-------------------------------------------
    // Aero
#if 0
    setAttribute(Qt::WA_TranslucentBackground);

    ::SetWindowLong(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION | WS_EX_LAYERED);

    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser)
    {
        pfnSetWindowCompositionAttribute setWindowCompositionAttribute = (pfnSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (setWindowCompositionAttribute)
        {
            ACCENT_POLICY accent = { ACCENT_ENABLE_BLURBEHIND, 0, 0, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data;
            data.Attrib = WCA_ACCENT_POLICY;
            data.pvData = &accent;
            data.cbData = sizeof(accent);
            setWindowCompositionAttribute(hwnd, &data);
        }
    }
#endif
}

MainUI::~MainUI()
{
#if 0
    delete leftBar;
    delete midBar;
    delete rightBar;
#endif
}

void MainUI::setSocket(ClientSocket *socket,ClientFileSocket *filesocket)
{
    if(socket != nullptr && filesocket != nullptr){
        tcpSocket = socket;

        connect(tcpSocket, SIGNAL(signalMessage(quint8,QJsonValue)),
                this, SLOT(sltTcpReply(quint8,QJsonValue)));
        connect(tcpSocket, SIGNAL(signalStatus(quint8)),
                this, SLOT(sltTcpStatus(quint8)));

        connect(rightBar, SIGNAL(signalSendMessage(const quint8 &,const QJsonValue &)),
                tcpSocket, SLOT(sltSendMessage(quint8,QJsonValue)));

        connect(midBar, SIGNAL(signalSendMessage(const quint8 &,const QJsonValue &)),
                tcpSocket, SLOT(sltSendMessage(quint8,QJsonValue)));
        connect(midBar,SIGNAL(signalAddChat(Cell*)),this,SLOT(sltAddChat(Cell*)));
        connect(tcpSocket,SIGNAL(signalFindFriendReply(const QJsonValue &)),
                midBar,SIGNAL(signalFindFriendReply(const QJsonValue &)));

        globalFileSocket = filesocket;
        connect(globalFileSocket,SIGNAL(signalFileRecvOk(quint8,QString,int)),
                this,SLOT(sltFileRecvFinished(quint8,QString,int)));

        initUI();
    }
}


void MainUI::initUI()
{
    //加载左栏的头像 ok
    midBar->InitContactList();//加载好友列表和群列表
    midBar->InitChatList();//加载中栏的历史聊天记录框
}

void MainUI::sltFileRecvFinished(quint8, QString,int)
{

}

void MainUI::paintEvent(QPaintEvent *e)
{
#if 0
    //画圆角
    QBitmap maskBmp(this->size());
    maskBmp.fill();
    QPainter maskBmpPainter(&maskBmp);
    maskBmpPainter.setPen(Qt::NoPen);
    maskBmpPainter.setBrush(Qt::black);
    maskBmpPainter.setRenderHint(QPainter::Antialiasing, true); //抗锯齿
    maskBmpPainter.drawRoundedRect(this->rect(), 15, 15);
    setMask(maskBmp);
#endif
    return QWidget::paintEvent(e);
}

//点击左栏按钮切换中栏
void MainUI::onBtnClicked(int page)
{
    midBar->SltMainPageChanged(page);

    if(page == 0){
        leftBar->chatList->onBtnClicked();
        leftBar->contacts->restoreBtn();
        leftBar->settings->restoreBtn();
        rightBar->restorePage();
    }else if(page == 1){
        leftBar->chatList->restoreBtn();
        leftBar->contacts->onBtnClicked();
        leftBar->settings->restoreBtn();
        rightBar->resetPage();
    }else if(page == 2){
//        leftBar->chatList->restoreBtn();
//        leftBar->contacts->restoreBtn();
//        leftBar->settings->onBtnClicked();
//        rightBar->resetPage();

        qDebug() << "显示设置面板";
        settings->show();
    }
}

void MainUI::stayOnTop(bool top)
{
    if(top)
        this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    else
        this->setWindowFlags(Qt::FramelessWindowHint | Qt::Widget);
    show();
}

void MainUI::sltTcpReply(quint8 type, QJsonValue dataVal)
{
    switch (type) {
    case UserOnLine:
    {
        //UpdateFriendStatus(OnLine, dataVal);
    }
        break;
    case UserOffLine:
    {
        //UpdateFriendStatus(OffLine, dataVal);
    }
        break;
    case UpdateHeadPic:
    {
        // 你的好友更新了头像
        //ParseUpFriendHead(dataVal);
    }
        break;
    case AddFriend:
    {
        parseAddFriendReply(dataVal);
    }
        break;
    case AddGroup:
    {
        parseAddGroupReply(dataVal);
    }
        break;

    case AddFriendRequist:
    {
        parseAddFriendRequestConfirmed(dataVal);
    }
        break;
    case AddGroupRequist:
    {
        parseAddGroupRequestConfirmed(dataVal);
    }
        break;

    case CreateGroup:
    {
        //ParseCreateGroupReply(dataVal);
    }
        break;
    case GetMyFriends:
    {
        //ParseGetFriendsReply(dataVal);
    }
        break;
    case GetMyGroups:
    {
        //ParseGetGroupFriendsReply(dataVal);
    }
        break;
    case GetGroupMembers:
    {
        parseGetGroupMembersReply(dataVal);
    }
        break;
    case RefreshFriends:
    {
        //ParseRefreshFriendsReply(dataVal);
    }
        break;
    case RefreshGroups:
    {
        //ParseRefreshGroupFriendsReply(dataVal);
    }
        break;
    case SendMsg:
    {
        parseFriendMessageReply(dataVal);//收到私聊消息
    }
        break;
    case SendGroupMsg:
    {
        parseGroupMessageReply(dataVal);//收到群聊消息
    }
        break;
    case SendFile:
    case SendPicture:
    {
        parseSendFileReply(dataVal);//收到服务器文件下载通知
    }
        break;
    case MsgReceived:
    {
        rightBar->msgconfirmed(dataVal);
    }
        break;
    case DeleteFriend:
    {
        parseDeleteFriendReply(dataVal);
    }
        break;
    case DeleteGroup:
    {
        parseDeleteGroupReply(dataVal);
    }
        break;
    default:
        break;
    }
}

void MainUI::sltTcpStatus(quint8)
{

}

void MainUI::sltAddChat(Cell *cell)
{
    if(cell->type == Cell_FriendChat){
        //切换左栏按钮
        onBtnClicked(0);

        //中栏插入聊天格子
        midBar->insertCell(cell);
        midBar->switchToChatList();

        //右栏聊天窗口增加一条消息
        QJsonObject jsonObj;
        jsonObj.insert("type",Notice);
        jsonObj.insert("noticeType",NewFriend);
        jsonObj.insert("id",cell->id);
        jsonObj.insert("msg","你和" + QString::number(cell->id) + "已经成为好友了，开始聊天吧！");

        rightBar->allowSendMsg(cell->id);//允许和此用户聊天
        rightBar->msgReceived(cell,jsonObj);

        //更新联系人列表
        Cell* c = new Cell;
        c->type = Cell_FriendContact;
        c->groupName = "默认分组";
        c->id = cell->id;
        c->iconPath = cell->iconPath;
        c->name = cell->name;
        c->isClicked = false;
        c->status = OnLine;
        midBar->addCellToContact(c);//更新联系人列表
    }else if(cell->type == Cell_GroupChat){
        //切换左栏按钮
        onBtnClicked(0);

        Cell* c = midBar->isIDExist(cell->id);
        if(c == nullptr){
            //说明中栏目前没有此聊天格子
            //中栏插入聊天格子
            midBar->insertCell(cell);
            midBar->switchToChatList();

            //右栏聊天窗口增加一条消息
            QJsonObject jsonObj;
            jsonObj.insert("type",Notice);
            jsonObj.insert("noticeType",NewMember);
            jsonObj.insert("id",cell->newUserID);
            jsonObj.insert("msg",cell->msg);

            rightBar->msgReceived(cell,jsonObj);
        }else{
            c->msg = cell->msg;
            c->subTitle = cell->subTitle;

            //刷新聊天窗口的群成员列表
            Cell* newUser = new Cell;
            newUser->type = Cell_FriendChat;
            newUser->id = cell->newUserID;
            newUser->name = cell->newUserName;
            newUser->iconPath = cell->newUserHead;
            newUser->status = OnLine;
            rightBar->addNewUserToGroupList(cell->id,newUser);

            //右栏聊天窗口增加一条消息
            QJsonObject jsonObj;
            jsonObj.insert("type",Notice);
            jsonObj.insert("noticeType",NewMember);
            jsonObj.insert("id",cell->newUserID);
            jsonObj.insert("msg",c->msg);
            rightBar->msgReceived(c,jsonObj);
        }
    }
}

void MainUI::parseFriendMessageReply(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();
        int senderID = json.value("id").toInt();//获取消息发送者的ID
        QString name = json.value("name").toString();

        Cell* cell = midBar->isIDExist(senderID);
        if(cell == nullptr){//证明聊天列表中没有和该用户的聊天记录
            cell = new Cell;
            cell->id = senderID;
            QJsonObject json = DataBase::Instance()->getFriendInfo(senderID);
            cell->name = json.value("name").toString();
            cell->iconPath = json.value("head").toString();
            cell->type = Cell_FriendChat;

            midBar->insertCell(cell);
            midBar->switchToChatList();
            //同时新增一个chatWindow
        }

        rightBar->msgReceived(cell,dataVal);
    }
}

void MainUI::parseGroupMessageReply(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();
        int groupid = json.value("group").toInt();
        QString groupName = json.value("groupName").toString();
        int type = json.value("type").toInt();

        //聊天列表是否存在该群的聊天记录
        Cell *cell = midBar->isIDExist(groupid);
        if(cell == nullptr){//证明聊天列表中没有和该用户的聊天记录
            cell = new Cell;
            cell->id = groupid;
            cell->name = groupName;

            QJsonObject json = DataBase::Instance()->getGroupInfo(groupid);
            cell->name = json.value("name").toString();
            cell->iconPath = json.value("head").toString();
            cell->type = Cell_GroupChat;

            midBar->insertCell(cell);
            midBar->switchToChatList();
        }

        rightBar->msgReceived(cell,dataVal);

        if(type == Notice){
            int noticeType = json.value("noticeType").toInt();
            if(noticeType == NewMember){
                //刷新聊天窗口的群成员列表
                Cell* newUser = new Cell;
                newUser->type = Cell_FriendChat;
                newUser->id = json.value("id").toInt();
                newUser->name = json.value("name").toString();
                newUser->iconPath = MyApp::m_strHeadPath +
                        json.value("head").toString();
                newUser->status = OnLine;
                rightBar->addNewUserToGroupList(groupid,newUser);
            }
        }
    }
}

void MainUI::parseSendFileReply(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();
        int tag = json.value("tag").toInt();
        int id = json.value("id").toInt();
        int groupid = json.value("group").toInt();
        Cell *cell = nullptr;
        if(tag == 0){//私发文件
            cell = midBar->isIDExist(id);
        }else if(tag == 1){//群发文件
            cell = midBar->isIDExist(groupid);
        }

        if(cell == nullptr){
            cell = new Cell;
            if(tag == 0){//私发文件
                cell->id = id;
                QJsonObject json = DataBase::Instance()->getFriendInfo(id);
                cell->name = json.value("name").toString();
                cell->iconPath = json.value("head").toString();
                cell->type = Cell_FriendChat;
            }else if(tag == 1){//群发文件
                cell->id = groupid;
                QJsonObject json = DataBase::Instance()->getGroupInfo(groupid);
                cell->name = json.value("name").toString();
                cell->iconPath = json.value("head").toString();
                cell->type = Cell_GroupChat;
            }
            midBar->insertCell(cell);
            midBar->switchToChatList();
        }

        rightBar->msgReceived(cell,dataVal);
    }
}

void MainUI::parseAddFriendReply(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();

        int id = json.value("id").toInt();
        QString head = json.value("head").toString();
        QString name = json.value("name").toString();

        Cell *cell = new Cell;
        cell->id = id;
        cell->iconPath = MyApp::m_strHeadPath + head;
        cell->name = name;
        cell->subTitle = QDateTime::currentDateTime().toString("hh:mm:ss");
        cell->msg = QString::number(id) + "请求添加好友";
        cell->type = Cell_AddFriend;

        midBar->insertCell(cell);
        midBar->switchToChatList();
    }
}

void MainUI::parseDeleteFriendReply(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();
        int id = json.value("id").toInt();
        midBar->deleteFriend(id);

        rightBar->forbidSendMsg(id);
    }
}

void MainUI::parseAddGroupReply(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();

        int id = json.value("id").toInt();
        QString name = json.value("name").toString();
        QString head = json.value("head").toString();

        int groupid = json.value("group").toInt();
        QString groupHead = json.value("groupHead").toString();
        QString groupName = json.value("groupName").toString();

        Cell *cell = new Cell;
        cell->id = id;
        cell->name = name;
        cell->iconPath = MyApp::m_strHeadPath + head;
        QFileInfo fileInfo(cell->iconPath);
        if(!fileInfo.exists() || head == ""){
            //头像文件不存在，向服务器获取
            qDebug() << cell->iconPath << "头像文件不存在，正在向服务器获取...";

            QJsonObject json;
            json.insert("tag",-2);
            json.insert("from",MyApp::m_nId);
            json.insert("id",-2);
            json.insert("who",cell->id);
            tcpSocket->sltSendMessage(GetPicture,json);

            myHelper::Sleep(500);//等待500毫秒
        }

        cell->groupid = groupid;
        cell->groupname_ = groupName;
        cell->groupHead = MyApp::m_strHeadPath + groupHead;

        cell->subTitle = QDateTime::currentDateTime().toString("hh:mm:ss");
        cell->msg = QString::number(id) + "请求加入群：" + QString::number(groupid);
        cell->type = Cell_AddGroup;

        midBar->insertCell(cell);
        midBar->switchToChatList();
    }
}

void MainUI::parseDeleteGroupReply(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();
        int groupid = json.value("group").toInt();
        QString groupName = json.value("groupName").toString();

        int id = json.value("id").toInt();
        QString name = json.value("name").toString();

        rightBar->removeUserFromGroupList(groupid,id);
    }
}

void MainUI::parseAddFriendRequestConfirmed(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();
        int id = json.value("id").toInt();
        QString name = json.value("name").toString();
        QString head = json.value("head").toString();

        //更新本地数据库
        QSqlQuery query;
        query.prepare("INSERT INTO MyFriend (id, name, head, subgroup, tag, lastMsg, lastTime) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?);");
        query.bindValue(0, id);
        query.bindValue(1, name);
        query.bindValue(2, MyApp::m_strHeadPath + head);
        query.bindValue(3, "默认分组");
        query.bindValue(4, 0);
        query.bindValue(5, "");
        query.bindValue(6, "");

        query.exec();

        Cell *cell = new Cell;
        cell->id = id;
        cell->name = name;
        cell->iconPath = MyApp::m_strHeadPath + head;
        cell->type = Cell_FriendChat;
        cell->status = OnLine;
        cell->msg = "你和" + QString::number(id) + "已经成为好友了！";
        cell->subTitle = QDateTime::currentDateTime().toString("hh:mm:ss");

        midBar->insertCell(cell);
        midBar->switchToChatList();

        QJsonObject jsonObj;
        jsonObj.insert("type",Notice);
        jsonObj.insert("noticeType",NewFriend);
        jsonObj.insert("id",id);
        jsonObj.insert("msg","你和" + QString::number(id) + "已经成为好友了，开始聊天吧！");

        rightBar->allowSendMsg(id);
        rightBar->msgReceived(cell,jsonObj);
        onBtnClicked(0);

        Cell* c = new Cell;
        c->type = Cell_FriendContact;
        c->groupName = "默认分组";
        c->id = cell->id;
        c->name = cell->name;
        c->iconPath = cell->iconPath;
        c->isClicked = false;
        c->status = OnLine;
        midBar->addCellToContact(c);
    }
}

void MainUI::parseAddGroupRequestConfirmed(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();
        int id = json.value("id").toInt();
        QString name = json.value("name").toString();
        QString head = json.value("head").toString();
        int adminID = json.value("adminID").toInt();

        //更新本地数据库
        QSqlQuery query;
        query.prepare("INSERT INTO MyGroup (id, name, admin, head, tag, lastMsg, lastTime) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?);");
        query.bindValue(0, id);
        query.bindValue(1, name);
        query.bindValue(2, adminID);
        query.bindValue(3, MyApp::m_strHeadPath + head);
        query.bindValue(4, 0);
        query.bindValue(5, 0);
        query.bindValue(6, 0);

        query.exec();

        Cell *cell = new Cell;
        cell->id = id;
        cell->name = name;
        cell->iconPath = MyApp::m_strHeadPath + head;
        cell->type = Cell_GroupChat;
        cell->status = OnLine;
        cell->msg = "你已加入该群";
        cell->subTitle = QDateTime::currentDateTime().toString("hh:mm:ss");

        midBar->insertCell(cell);
        midBar->switchToChatList();

        QJsonObject jsonObj;
        jsonObj.insert("type",Notice);
        jsonObj.insert("noticeType",NewMember);
        jsonObj.insert("group",cell->id);
        jsonObj.insert("id",MyApp::m_nId);
        jsonObj.insert("msg",QString("你已经加入该群，") + "开始聊天吧！");

        rightBar->msgReceived(cell,jsonObj);
        onBtnClicked(0);

        Cell* c = new Cell;
        c->type = Cell_GroupContact;
        c->groupName = "我的群组";
        c->id = cell->id;
        c->name = cell->name;
        c->iconPath = cell->iconPath;
        c->isClicked = false;
        c->status = OnLine;

        midBar->addCellToContact(c);
    }
}

void MainUI::parseGetGroupMembersReply(const QJsonValue &dataVal)
{
    qDebug() << "收到服务器发来的群成员: " << endl
             << dataVal;
    rightBar->refreshGroupList(dataVal);
}

bool MainUI::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = (MSG*)message;
    switch (msg->message)
    {
    case WM_NCCALCSIZE:
    {
        // this kills the window frame and title bar we added with WS_THICKFRAME and WS_CAPTION
        *result = 0;
        return true;
    }
    default:
        return QWidget::nativeEvent(eventType, message, result);
    }
}
