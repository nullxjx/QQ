#include "clientsocket.h"
#include "unit.h"
#include "myapp.h"
#include "database.h"
#include "global.h"

#include <QFile>
#include <QDebug>
#include <QHostAddress>
#include <QDataStream>
#include <QApplication>
#include <QDateTime>
#include <QFileInfo>

ClientSocket::ClientSocket(QObject *parent) : QObject(parent)
{
    ID = -1;

    tcpSocket = new QTcpSocket(this);

    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(sltReadyRead()));
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(sltConnected()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(sltDisconnected()));
}

ClientSocket::~ClientSocket()
{
    sltSendOffline();
}

int ClientSocket::getUserID() const
{
    return  ID;
}

void ClientSocket::setUserID(const int &id)
{
    ID = id;
}

void ClientSocket::checkConnected()
{
    if (tcpSocket->state() != QTcpSocket::ConnectedState){
        qDebug() << "Connecting to server...";
        myHelper::printLogFile("Connecting to server...");

        tcpSocket->connectToHost(MyApp::m_strHostAddr, quint16(MyApp::m_nMsgPort));
    }
    qDebug() << "Connected to server!";
    myHelper::printLogFile("Connected to server!");
}

void ClientSocket::closeConnection()
{
    if (tcpSocket->isOpen())
        tcpSocket->abort();
}

void ClientSocket::connectToHost(const QString &host, const int &port)
{
    if (tcpSocket->isOpen())
        tcpSocket->abort();
    tcpSocket->connectToHost(host, quint16(port));
}

void ClientSocket::connectToHost(const QHostAddress &host, const int &port)
{
    if (tcpSocket->isOpen())
        tcpSocket->abort();
    tcpSocket->connectToHost(host, quint16(port));
}

void ClientSocket::sltSendMessage(const quint8 &type, const QJsonValue &dataVal)
{
    // 连接服务器
    if (!tcpSocket->isOpen()) {
        tcpSocket->connectToHost(MyApp::m_strHostAddr, quint16(MyApp::m_nMsgPort));
        tcpSocket->waitForConnected(1000);
    }

    // 超时1s后还是连接不上，直接返回
    if (!tcpSocket->isOpen())
        return;

    // 构建 Json 对象
    QJsonObject json;
    json.insert("from", ID);
    json.insert("type", type);
    json.insert("data", dataVal);

    // 构建 Json 文档
    QJsonDocument document;
    document.setObject(json);
    tcpSocket->write(document.toJson(QJsonDocument::Indented));

    qDebug() << "-> 向服务器发送一条消息，消息内容为：" << endl << json << endl;
}

void ClientSocket::sltSendOnline()
{
    // 上线的时候给当前好友上报下状态
    QJsonArray friendArr = DataBase::Instance()->getMyFriends();

    // 组织Jsonarror
    sltSendMessage(UserOnLine, friendArr);
}

void ClientSocket::sltDisconnected()
{
    qDebug() << "disconnecetd to server";
    tcpSocket->abort();
    Q_EMIT signalStatus(DisConnectedHost);
}

void ClientSocket::sltConnected()
{
    qDebug() << "connecetd to server";
    myHelper::printLogFile("connecetd to server");
    Q_EMIT signalStatus(ConnectedHost);
}

void ClientSocket::sltReadyRead()
{
    // 读取socket数据
    QByteArray byRead = tcpSocket->readAll();

    QJsonParseError jsonError;
    // 转化为 JSON 文档
    QJsonDocument doucment = QJsonDocument::fromJson(byRead, &jsonError);
    // 解析未发生错误
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {
        // JSON 文档为对象
        if (doucment.isObject()) {
            // 转化为对象
            QJsonObject jsonObj = doucment.object();
            qDebug() << "<- 收到服务器发来的消息，消息内容为：" << endl << jsonObj << endl;

            QJsonValue dataVal = jsonObj.value("data");

            int msgType = jsonObj.value("type").toInt();

            // 根据消息类型解析服务器消息
            switch (msgType) {
            case Register:
            {
                Q_EMIT signalRegisterOk(dataVal);
            }
                break;
            case Login:
            {
                parseLogin(dataVal);
            }
                break;
            case UserOnLine:
            {
                qDebug() << "user is oline" << dataVal;
                Q_EMIT signalMessage(UserOnLine, dataVal);
            }
                break;
            case UserOffLine:
            {
                qDebug() << "user is offline" << dataVal;
                Q_EMIT signalMessage(UserOffLine, dataVal);
            }
                break;
            case Logout:
            {
                tcpSocket->abort();
            }
                break;
            case UpdateHeadPic:
            {
                Q_EMIT signalMessage(UpdateHeadPic, dataVal);
            }
                break;
            case AddFriend:
            {
                Q_EMIT signalMessage(AddFriend, dataVal);
            }
                break;
            case AddGroup:
            {
                Q_EMIT signalMessage(AddGroup, dataVal);
            }
                break;
            case AddFriendRequist:
            {
                Q_EMIT signalMessage(AddFriendRequist, dataVal);
            }
                break;
            case AddGroupRequist:
            {
                Q_EMIT signalMessage(AddGroupRequist, dataVal);
            }
                break;
            case CreateGroup:
            {
                Q_EMIT signalMessage(CreateGroup, dataVal);
            }
                break;
            case GetMyFriends:
            {
                Q_EMIT signalMessage(GetMyFriends, dataVal);
            }
                break;
            case GetMyGroups:
            {
                Q_EMIT signalMessage(GetMyGroups, dataVal);
            }
                break;
            case GetGroupMembers:
            {
                Q_EMIT signalMessage(GetGroupMembers, dataVal);
            }
                break;
            case RefreshFriends:
            {
                Q_EMIT signalMessage(RefreshFriends, dataVal);
            }
                break;
            case RefreshGroups:
            {
                Q_EMIT signalMessage(RefreshGroups, dataVal);
            }
                break;
            case SendMsg:
            {
                Q_EMIT signalMessage(SendMsg, dataVal);
            }
                break;
            case SendGroupMsg:
            {
                Q_EMIT signalMessage(SendGroupMsg, dataVal);
            }
                break;
            case SendFile:
            {
                Q_EMIT signalMessage(SendFile, dataVal);
            }
                break;
            case SendPicture:
            {
                Q_EMIT signalMessage(SendPicture, dataVal);
            }
                break;

            case MsgReceived:
            {
                Q_EMIT signalMessage(MsgReceived, dataVal);
            }
                break;
            case ChangePasswd:
            {
                Q_EMIT signalChangePwdReply(dataVal);
            }
                break;
            case FindFriend:
            {
                Q_EMIT signalFindFriendReply(dataVal);
            }
                break;
            case DeleteFriend:
            {
                Q_EMIT signalMessage(DeleteFriend,dataVal);
            }
                break;
            case DeleteGroup:
            {
                Q_EMIT signalMessage(DeleteGroup,dataVal);
            }
                break;
            case GetOfflineMsg:
            {
                Q_EMIT signalGetOfflineMsg(dataVal);
            }
                break;
            default:
                break;
            }
        }
    }
}

void ClientSocket::parseLogin(const QJsonValue &dataVal)
{
    // data 的 value 是对象
    if (dataVal.isObject()) {
        QJsonObject dataObj = dataVal.toObject();
        int code = dataObj.value("code").toInt();

        if (0 == code) {
            MyApp::m_nId = dataObj.value("id").toInt();
            MyApp::m_strUserName = dataObj.value("name").toString();
            MyApp::m_strHeadFile = dataObj.value("head").toString();//服务器发来的头像名

            ID = MyApp::m_nId;
            Q_EMIT signalStatus(LoginSuccess);
        } else if (-1 == code) {
            Q_EMIT signalStatus(LoginPasswdError);
        } else if (-2 == code) {
            Q_EMIT signalStatus(UserNotExist);
        } else if (-3 == code) {
            Q_EMIT signalStatus(LoginRepeat);
        }
    }
}

void ClientSocket::sltSendOffline()
{
    QJsonObject json;
    json.insert("id", ID);
    QJsonArray friendArr = DataBase::Instance()->getMyFriends();
    json.insert("friends", friendArr);

    // 通知我的好友，我下线了
    this->sltSendMessage(Logout, json);
}


//------------------------------------------------------


ClientFileSocket::ClientFileSocket(QObject *parent) : QObject(parent)
{
    type = Login;

    strFilePath = MyApp::m_strRecvPath;

    initSocket();
}

ClientFileSocket::~ClientFileSocket()
{
}

bool ClientFileSocket::isConneciton()
{
    return tcpSocket->isOpen();
}

void ClientFileSocket::displayError(QAbstractSocket::SocketError)
{
    tcpSocket->close();
}

void ClientFileSocket::startTransferFile(QString fileName,qint64 time,quint8 type_)
{
    if (busy)
        return;
    type = type_;

    // 当有数据发送成功时，我们更新进度条
    connect(tcpSocket, SIGNAL(bytesWritten(qint64)),this, SLOT(sltUpdateClientProgress(qint64)));

    // 如果没有连接服务器，重新连接下
    if (!tcpSocket->isOpen()) {
        connectToServer(MyApp::m_strHostAddr, MyApp::m_nFilePort, winID);
    }

    // 要发送的文件
    fileToSend = new QFile(fileName);

    if (!fileToSend->open(QFile::ReadOnly)){
        qDebug() << "open file error!";
        myHelper::printLogFile("open file error!");
        return;
    }

    // 文件总大小
    ullSendTotalBytes = qint64(fileToSend->size());

    // 文件数据流
    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_11);

    fileSendName = fileName;

    // 当前文件名，不包含路径
    QString currentFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);

    // 依次写入总大小信息空间，文件名大小信息空间，文件发送时间戳，文件名
    sendOut << qint64(0) << qint64(0) << qint64(0) << currentFileName;

    // 这里的总大小是文件名大小等信息和实际文件大小的总和
    ullSendTotalBytes += outBlock.size();

    // 返回outBolock的开始，用实际的大小信息代替两个qint64(0)空间
    sendOut.device()->seek(0);
    //头部依次写入 发送数据总大小,文件名大小
    sendOut << ullSendTotalBytes << qint64(unsigned(outBlock.size()) - sizeof(qint64)*3) << time;


    // 发送完头数据后剩余数据的大小
    bytesToWrite = quint64(ullSendTotalBytes - tcpSocket->write(outBlock));

    outBlock.resize(0);
    busy = true;
}

void ClientFileSocket::connectToServer(const QString &ip, const int &port, const int &winId)
{
    if (tcpSocket->isOpen())
        return;
    winID = winId;
    tcpSocket->connectToHost(QHostAddress(ip), quint16(port));
}

void ClientFileSocket::closeConnection()
{
    // 还原变量
    fileTransFinished();

    // 主动断开
    tcpSocket->abort();
}

void ClientFileSocket::fileTransFinished()
{
    busy = false;

    ullSendTotalBytes   = 0;
    ullRecvTotalBytes   = 0;
    bytesWritten        = 0;
    bytesToWrite        = 0;
    bytesReceived       = 0;

    fileNameSize        = 0;
    //qDebug() << "file transfer finished!";
}

void ClientFileSocket::setUserId(const int &id)
{
    winID = id;
}

void ClientFileSocket::initSocket()
{
    // 将整个大的文件分成很多小的部分进行发送，每部分为50字节
    loadSize            = 50 * 1024;
    ullSendTotalBytes   = 0;
    ullRecvTotalBytes   = 0;
    bytesWritten        = 0;
    bytesToWrite        = 0;
    bytesReceived       = 0;

    winID            = -1;

    fileNameSize        = 0;
    busy = false;

    fileToSend = new QFile(this);
    fileToRecv = new QFile(this);

    tcpSocket = new QTcpSocket(this);

    // 当有数据接收成功时，我们更新进度条
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(sltReadyRead()));
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(sltConnected()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(sltDisConnected()));
}

void ClientFileSocket::sltUpdateClientProgress(qint64 numBytes)
{
    // 开始的时候，不记录流程
    if (Login == type) {
        type = Unknow;
        return;
    }

    // 已经发送数据的大小
    bytesWritten += numBytes;
    if (bytesToWrite > 0) {//如果已经发送了数据
        // 每次发送loadSize大小的数据，这里设置为4KB，如果剩余的数据不足4KB，就发送剩余数据的大小
        outBlock = fileToSend->read(qint64((qMin(bytesToWrite, loadSize))));

        // 发送完一次数据后还剩余数据的大小
        bytesToWrite -= quint64(tcpSocket->write(outBlock));

        // 清空发送缓冲区
        outBlock.resize(0);
    }else{
        // 如果没有发送任何数据，则关闭文件
        if (fileToSend->isOpen())
            fileToSend->close();
    }

    // 发送进度信息
    if(winID != -2)
        Q_EMIT signalUpdateProgress(bytesWritten, ullSendTotalBytes);

    // 发送完毕
    if (bytesWritten >= ullSendTotalBytes)
    {
        if (fileToSend->isOpen())
            fileToSend->close();

        bytesWritten = 0;
        ullSendTotalBytes = 0;
        bytesToWrite = 0;
        // 文件发送完成，复位相关信息
        if(winID != -2)
            Q_EMIT signalSendFinished(type,fileSendName);

        fileTransFinished();
    }
}

void ClientFileSocket::sltReadyRead()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_5_11);

    // 如果接收到的数据小于等于32个字节，那么是刚开始接收数据，我们保存为头文件信息
    if (bytesReceived <= (sizeof(qint64)*5))
    {
        int nlen = sizeof(qint64) * 5;
        // 接收数据总大小信息和文件名大小信息
        if ((tcpSocket->bytesAvailable() >= nlen) && (fileNameSize == 0))
        {
            in >> ullRecvTotalBytes >> fileNameSize >> senderID >> RecvFileSendTime >> flag;
            if (0 != ullRecvTotalBytes) bytesReceived += nlen;
        }

        // 接收文件名，并建立文件
        if((tcpSocket->bytesAvailable() >= qint64(fileNameSize))&&
                (qint64(fileNameSize) != 0) && (0 != ullRecvTotalBytes)) {

            in >> fileReadName;
            qDebug() << "fileReadName:" << fileReadName;
            if(winID == -2){
                if(flag == -1){
                    fileReadName = MyApp::m_strRecvPath + fileReadName;
                }else{
                    fileReadName = MyApp::m_strHeadPath + fileReadName;
                }
            }else{
                fileReadName = MyApp::m_strRecvPath + fileReadName;
            }

            bytesReceived += fileNameSize;

            fileToRecv->setFileName(fileReadName);

            if (!fileToRecv->open(QFile::WriteOnly | QIODevice::Truncate)){
                qDebug() << "recv file open failed" << fileReadName;
                myHelper::printLogFile("recv file open failed");
                return;
            }

            qDebug() << "Begin to recv file" << fileReadName << "sent by:"
                     << (senderID == 0 ? QString::number(winID)
                                       : QString::number(senderID) + " in group " + QString::number(winID));
            myHelper::printLogFile("Begin to recv file" + fileReadName + "sent by:" +
                                   (senderID == 0 ? QString::number(winID)
                                  : QString::number(senderID) + " in group " + QString::number(winID)));

            QFileInfo fileinfo(fileReadName);
            if(fileinfo.suffix().toLower() == "png" || fileinfo.suffix().toLower() == "jpg"){//证明收到的是图片
                //emit signalFileArrived(Picture,winID);//通知聊天窗口在聊天界面插入一个气泡
            }else{

                QJsonObject json;
                json.insert("tag",tag);
                json.insert("type",Files);
                json.insert("from",senderID);
                json.insert("winID",winID);//senderID != 0表示是群聊,需要再把群id写入
                json.insert("time",RecvFileSendTime);

                if(winID != -2)
                    emit signalFileArrived(json);//更新聊天窗口相应气泡框的进度条
            }
        }
    }

    // 如果接收的数据小于总数据，那么写入文件
    if (bytesReceived < ullRecvTotalBytes)
    {
        bytesReceived += tcpSocket->bytesAvailable();
        inBlock = tcpSocket->readAll();

        if (fileToRecv->isOpen())
            fileToRecv->write(inBlock);

        inBlock.resize(0);
    }

    // 更新进度条
    QFileInfo fileinfo(fileReadName);
    if(fileinfo.suffix().toLower() != "png" && fileinfo.suffix().toLower() != "jpg" && tag != -2){
        //注意收到图片是不需要更新进度条的
        if(winID != -2)
            Q_EMIT signalUpdateProgress(bytesReceived, ullRecvTotalBytes);
    }

    // 接收数据完成时
    if ((bytesReceived >= ullRecvTotalBytes) && (0 != ullRecvTotalBytes))
    {
        fileToRecv->close();
        bytesReceived = 0; // clear for next receive
        ullRecvTotalBytes = 0;
        fileNameSize = 0;

        QFileInfo fileinfo(fileReadName);
        if(fileinfo.suffix().toLower() == "png" || fileinfo.suffix().toLower() == "jpg"){//证明收到的是图片
            if(winID != -2)
                Q_EMIT signalFileRecvOk(Picture, fileToRecv->fileName(), tag == 0?0:int(senderID));
        }else{
            if(winID != -2)
                Q_EMIT signalFileRecvOk(Files, fileToRecv->fileName(), tag == 0?0:int(senderID));
        }
        qDebug() << "File recv success!" << fileToRecv->fileName() << endl;

        // 数据接受完成
        fileTransFinished();
    }
}

void ClientFileSocket::sltConnected()
{
    type = Login;

    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_11);

    // 给服务器socket上报自己的id和聊天窗口中对方的ID，方便下次查询
    sendOut << qint32(MyApp::m_nId) << qint32(winID) << qint32(tag);

    // 发送完头数据后剩余数据的大小
    tcpSocket->write(outBlock);

    // 发送连接上的信号
    Q_EMIT signalConnectd();
}

void ClientFileSocket::sltDisConnected()
{
    if (tcpSocket->isOpen())
        tcpSocket->close();
}
