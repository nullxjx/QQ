/****************************************************************************
** QQ-Like 登录界面窗体
****************************************************************************/

#include "loginwnd.h"
#include "mainui.h"
#include "myapp.h"
#include "global.h"
#include "bubbleinfo.h"
#include "database.h"

#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>

LoginWnd::LoginWnd(QWidget *parent): QStackedWidget(parent)
{
    this->setFixedSize(430,330);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setContentsMargins(0,0,0,0);

    m_notifyMsg= new QLabel(this);
    m_notifyMsg->setFixedSize(430, 20);
    m_notifyMsg->setStyleSheet("background-color:#09a3dc;font-size: 12px;font-family:Microsoft YaHei;border-radius:10px");
    m_notifyMsg->hide();


    //初始化TcpSocket
    tcpSocket = new ClientSocket();
    tcpSocket->connectToHost(MyApp::m_strHostAddr, MyApp::m_nMsgPort);
    connected = false;

    connect(tcpSocket, SIGNAL(signalStatus(quint8)), this, SLOT(sltTcpStatus(quint8)));
    connect(tcpSocket, SIGNAL(signalRegisterOk(const QJsonValue &)),
            this, SIGNAL(signalRegisterOK(const QJsonValue&)));
    connect(tcpSocket, SIGNAL(signalChangePwdReply(const QJsonValue &)),
            this, SIGNAL(signalChangePwdReply(const QJsonValue&)));
    connect(tcpSocket, SIGNAL(signalGetOfflineMsg(const QJsonValue&)),
            this,SLOT(writeOffLineMsgToDatabase(const QJsonValue&)));

    timer = new QTimer();
    timer->setInterval(1000);//每隔1秒尝试重新连接服务器
    connect(timer,SIGNAL(timeout()),this,SLOT(slotTimeout()));


    loginMainPage = new LoginWndMainPage;
    connect(loginMainPage,SIGNAL(loginSignal()),this,SLOT(changePage()));
    connect(loginMainPage,SIGNAL(showNotifyMsg(QString)),this,SLOT(showNotifyMsg(QString)));
    connect(loginMainPage,SIGNAL(hideNotifyMsg()),this,SLOT(hideNotifyMsg()));
    connect(loginMainPage,SIGNAL(closeWindow()),this,SIGNAL(closeWindow()));
    connect(loginMainPage,SIGNAL(hideWindow()),this,SIGNAL(hideWindow()));
    connect(loginMainPage,SIGNAL(rotateWindow()),this,SIGNAL(rotateWindow()));
    connect(loginMainPage,SIGNAL(openRegisterWnd()),this,SIGNAL(openRegisterWnd()));
    connect(loginMainPage,SIGNAL(openChangePasswordWnd()),
            this,SIGNAL(openChangePasswordWnd()));


    loginingPage = new LoginWndLoginingPage;
    connect(loginingPage,SIGNAL(cancelSignal()),this,SLOT(changePage()));
    connect(loginingPage,SIGNAL(closeWindow()),this,SIGNAL(closeWindow()));
    connect(loginingPage,SIGNAL(animationFinished()),this,SLOT(sltGetOffLineMsg()));
    connect(this,SIGNAL(loginSuccess()),loginingPage,SLOT(loginSuccess()));
    connect(loginMainPage,SIGNAL(changeLoginingHead()),
            loginingPage,SLOT(updateHead()));


    this->addWidget(loginMainPage);
    this->addWidget(loginingPage);
}


void LoginWnd::sltTcpStatus(const quint8 &state)
{
    switch (state) {
    case DisConnectedHost: {
        connected = false;
        showNotifyMsg(tr("服务器已断开"));
        break;
    }

    case ConnectedHost:{
        connected = true;
        timer->stop();
        showNotifyMsg(tr("已连接服务器"));
        break;
    }

    case LoginSuccess:{
        // 登陆验证成功
        disconnect(tcpSocket, SIGNAL(signalStatus(quint8)), this, SLOT(sltTcpStatus(quint8)));
        hideNotifyMsg();

        // 登录成功后，保存当前用户
        MyApp::m_nId = loginMainPage->getID().toInt();
        MyApp::m_strPassword = loginMainPage->getPassword();
        MyApp::saveConfig();

        //全局收发文件的socket,比如用户获取自己的头像，好友和群的头像等
        globalFileSocket = new ClientFileSocket();
        globalFileSocket->connectToServer(MyApp::m_strHostAddr,MyApp::m_nFilePort,-2);
        connect(globalFileSocket,SIGNAL(signalFileRecvOk(quint8,QString,int)),
                this,SLOT(sltFileRecvFinished(quint8,QString,int)));
        myHelper::Sleep(1000);

        //如果头像不存在，则向服务器获取头像
        if (!QFile::exists(MyApp::m_strHeadPath + MyApp::m_strHeadFile) || MyApp::m_strHeadFile.isEmpty()) {

            qDebug() << MyApp::m_strHeadPath + MyApp::m_strHeadFile
                     << "头像文件不存在，正在向服务器获取...";

            QJsonObject json;
            json.insert("tag",-2);
            json.insert("from", MyApp::m_nId);
            json.insert("id", -2);
            json.insert("who", MyApp::m_nId);

            tcpSocket->sltSendMessage(GetPicture, json);

            myHelper::Sleep(500);
        }

        emit loginSuccess();
        break;
    }

    case LoginPasswdError:{
        //QMessageBox::information(this, "错误", "登录失败，密码错误！");
        emit loginingPage->cancelSignal();
        showNotifyMsg(tr("登录失败，密码错误！"));
        break;
    }

    case LoginRepeat:{
        //QMessageBox::information(this, "错误", "登录失败，该账户已登录！");
        emit loginingPage->cancelSignal();
        showNotifyMsg(tr("登录失败，该账户已登录！"));
        break;
    }

    case UserNotExist:{
        //QMessageBox::information(this, "错误", "登录失败，该账户不存在！");
        emit loginingPage->cancelSignal();
        showNotifyMsg(tr("登录失败，该账户不存在！"));
        break;
    }

    case RegisterOk:{
        QMessageBox::information(this,"注册成功", "该账号注册成功！请点击登录！");
        break;
    }

    case RegisterFailed:{
        QMessageBox::information(this, "错误", "该账号已经注册！请点击登录！");
        break;
    }

    default:
        break;
    }
}


void LoginWnd::slotTimeout()
{
    if(!connected)
        tcpSocket->connectToHost(MyApp::m_strHostAddr, MyApp::m_nMsgPort);
}

void LoginWnd::sltFileRecvFinished(quint8,QString,int)
{
    qDebug() << "globalFileSocket 接受文件成功";
    myHelper::printLogFile("globalFileSocket 接受文件成功");
    MyApp::m_strHeadFile = QString::number(MyApp::m_nId) + ".png";
}

void LoginWnd::checkAutoLogin()
{
    loginMainPage->checkAutoLogin();
}

void LoginWnd::sltRegister(const QJsonValue &json)
{
    tcpSocket->sltSendMessage(Register, json);
}

void LoginWnd::sltChangePwd(const QJsonValue &json)
{
    tcpSocket->sltSendMessage(ChangePasswd,json);
}

void LoginWnd::writeOffLineMsgToDatabase(const QJsonValue &dataVal)
{
    qDebug() << "recv offline msg success,begin to write offline msg to database,time:"
             << QDateTime::currentDateTime().toString("yyyy-MM-dd  hh:mm:ss.zzz");

    int cnt = 0;
    if (dataVal.isArray()) {
        QJsonArray array = dataVal.toArray();
        cnt = array.size();
        for(int i = 0; i < cnt;i++){
            QJsonObject json = array.at(i).toObject();
            //qDebug() << "offline msg:" << json;
            int senderID = json.value("senderID").toInt();
            int receiverID = json.value("receiverID").toInt();
            int groupID = json.value("groupID").toInt();
            int type = json.value("type").toInt();
            qint64 time = json.value("time").toInt();
            QString msg = json.value("msg").toString();
            int tag = json.value("tag").toInt();
            qint64 fileSize = json.value("fileSize").toInt();

            BubbleInfo *info = new BubbleInfo;
            info->sender = You;
            info->myID = receiverID;
            info->yourID = senderID;
            info->groupID = groupID;
            info->tag = tag;
            info->msg = msg;
            info->fileSize = fileSize;
            info->downloaded = false;
            info->time = time;

            if(type == SendMsg || type == SendGroupMsg){
                info->msgType = Text;
            }else if(type == SendFile){
                info->msgType = Files;
            }else if(type == SendPicture){
                info->msgType = Picture;
                //需要主动接收图片
                QJsonObject json;
                json.insert("from",info->yourID);
                json.insert("to",info->myID);
                json.insert("group",info->groupID);
                json.insert("time",info->time);
                json.insert("tag",info->tag);
                json.insert("flag",-1);//表示该文件由gloabalFileSocket接收，不是由聊天窗口的fileSocket接收

                tcpSocket->sltSendMessage(GetFile,json);//向服务器发送消息获取图片文件
                info->downloaded = true;
                info->msg = MyApp::m_strRecvPath + info->msg;//记录图片地址
            }else{
                //目前不支持其他离线消息的加载
                continue;
            }

            DataBase::Instance()->addHistoryMsg(info);//把一条离线消息添加到本地历史记录中
        }
    }

    if(cnt == 0){
        qDebug() << "no offline msg available" << endl;
    }else{
        qDebug() << "write offline msg to database success,begin to show MainUI,time:"
                 << QDateTime::currentDateTime().toString("yyyy-MM-dd  hh:mm:ss.zzz")
                 << endl;
    }

    showMainUI();
}

void LoginWnd::sltGetOffLineMsg()
{
    //加载离线消息
    QJsonObject json;
    json.insert("id",MyApp::m_nId);
    json.insert("msg","request for loading offline msg");
    qDebug() << "begin to load offline msg from server,time:"
             << QDateTime::currentDateTime().toString("yyyy-MM-dd  hh:mm:ss.zzz");
    tcpSocket->sltSendMessage(GetOfflineMsg,json);

    myHelper::Sleep(2000);//等待2秒
}

void LoginWnd::paintEvent(QPaintEvent *e)
{
    QBitmap maskBmp(this->size());
    maskBmp.fill();
    QPainter maskBmpPainter(&maskBmp);
    maskBmpPainter.setPen(Qt::NoPen);
    maskBmpPainter.setBrush(Qt::black);
    maskBmpPainter.setRenderHint(QPainter::Antialiasing, true); //抗锯齿
    maskBmpPainter.drawRoundedRect(this->rect(), 10, 1);
    setMask(maskBmp);

    return QStackedWidget::paintEvent(e);
}

void LoginWnd::changePage()
{
    if(sender() == loginingPage){
        setCurrentWidget(loginMainPage);
    } else {
        if(connected){
            //向服务器发送登陆消息，同时登陆界面切换到登录中界面
            QJsonObject json;
            tcpSocket->setUserID(loginMainPage->getID().toInt());
            json.insert("id",loginMainPage->getID().toInt());
            json.insert("passwd",loginMainPage->getPassword());
            tcpSocket->sltSendMessage(Login,json);

            setCurrentWidget(loginingPage);
        }else{
            showNotifyMsg(tr("未连接至服务器，请检查服务器地址是否配置正确，正在尝试重新连接..."));

            timer->start();
        }
    }
}

void LoginWnd::showNotifyMsg(QString msg)
{
    this->setFixedSize(430, 350);
    m_notifyMsg->move(0, 330);
    m_notifyMsg->setText("  " + msg);
    m_notifyMsg->show();
}

void LoginWnd::hideNotifyMsg()
{
    this->setFixedSize(430, 330);
    m_notifyMsg->hide();
}

void LoginWnd::showMainUI()
{
    MainUI *mainUI = new MainUI;
    disconnect(globalFileSocket,SIGNAL(signalFileRecvOk(quint8,QString,int)),
            this,SLOT(sltFileRecvFinished(quint8,QString,int)));
    mainUI->setSocket(tcpSocket,globalFileSocket);

    qDebug() << "login success! close Login Window..." << endl
             << ".........................................................................."
             << endl << endl;

    qDebug() << "show MainUI..." << endl << endl;
    mainUI->show();

    emit closeWindow();
}
