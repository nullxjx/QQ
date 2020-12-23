#include "tcpserver.h"
#include "database.h"
#include "unit.h"
#include "myapp.h"

#include <QHostAddress>
#include <QDebug>
#include <QSqlError>
#include <QDateTime>

TcpServer::TcpServer(QObject *parent) : QObject(parent)
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(sltNewConnection()));
}

TcpServer::~TcpServer()
{
    if (tcpServer->isListening())
        tcpServer->close();
}

bool TcpServer::startListen(int port)
{
    if (tcpServer->isListening())
        tcpServer->close();
    return tcpServer->listen(QHostAddress::Any, quint16(port));
}

void TcpServer::closeListen()
{
    tcpServer->close();
}


//-----------------------------------------


TcpMsgServer::TcpMsgServer(QObject *parent) : TcpServer(parent)
{
}

TcpMsgServer::~TcpMsgServer()
{
    qDebug() << "tcp Msg server close...";
    foreach (ClientSocket *client, clients) {
        clients.removeOne(client);
        client->close();
    }
}

// 有新的客户端连接进来
void TcpMsgServer::sltNewConnection()
{
    ClientSocket *client = new ClientSocket(this, tcpServer->nextPendingConnection());
    qDebug() << "new msg socket connected,client info:" << client->getClientInfo() << endl;

    connect(client, SIGNAL(signalConnected()), this, SLOT(sltConnected()));
    connect(client, SIGNAL(signalDisConnected()), this, SLOT(sltDisConnected()));
}

void TcpMsgServer::sltConnected()
{
    ClientSocket *client = static_cast<ClientSocket*>(this->sender());
    if (client == nullptr)
        return;

    connect(client, SIGNAL(signalMsgToClient(quint8,int,QJsonValue)),
            this, SLOT(sltMsgToClient(quint8,int,QJsonValue)));
    connect(client, SIGNAL(signalDownloadFile(QJsonValue)), this, SIGNAL(signalDownloadFile(QJsonValue)));

    Q_EMIT signalUserStatus(QString("用户 [%1] 上线").
                            arg(DataBase::Instance()->getUserName(client->getUserID())));
    clients.push_back(client);
}

void TcpMsgServer::sltDisConnected()
{
    ClientSocket *client = static_cast<ClientSocket*>(this->sender());
    if (client == nullptr) return;

    for (int i = 0; i < clients.size(); i++) {
        if (client == clients.at(i)) {
            clients.remove(i);
            Q_EMIT signalUserStatus(QString("用户 [%1] 下线").
                                    arg(DataBase::Instance()->getUserName(client->getUserID())));

            return;
        }
    }

    disconnect(client, SIGNAL(signalConnected()), this, SLOT(sltConnected()));
    disconnect(client, SIGNAL(signalDisConnected()), this, SLOT(sltDisConnected()));
    disconnect(client, SIGNAL(signalMsgToClient(quint8,int,QJsonValue)),
               this, SLOT(sltMsgToClient(quint8,int,QJsonValue)));
    disconnect(client, SIGNAL(signalDownloadFile(QJsonValue)),
               this, SIGNAL(signalDownloadFile(QJsonValue)));
}

void TcpMsgServer::sltMsgToClient(const quint8 &type, const int &receiverID, const QJsonValue &jsonVal)
{
    // 查找要发送过去的id
    for (int i = 0; i < clients.size(); i++) {
        if (receiverID == clients.at(i)->getUserID()){
            clients.at(i)->sltSendMessage(type, jsonVal);
            return;
        }
    }

    //服务器端没有查询到该消息对应的接受者，说明该用户当前不在线，需要先把数据记录在服务器，等到用户上线后再发送
    if(jsonVal.isObject()){
        QJsonObject json = jsonVal.toObject();
        int senderID = json.value("id").toInt();
        qint64 time = json.value("time").toInt();
        QString msg = json.value("msg").toString();
        int tag = json.value("tag").toInt();
        int groupID = 0;
        if(tag == 1)
            groupID = json.value("group").toInt();
        qint64 fileSize = json.value("fileSize").toInt();

        QSqlQuery query;
        query.prepare("INSERT INTO UnreadMsg (senderID, receiverID, groupID, type, time, msg, tag, fileSize) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?);");
        query.bindValue(0, senderID);
        query.bindValue(1, receiverID);
        query.bindValue(2, tag == 0 ? 0 : groupID);//如果为私聊消息，该字段无效，设为0即可
        query.bindValue(3, type);
        query.bindValue(4, time);
        query.bindValue(5, msg);
        query.bindValue(6, tag);
        query.bindValue(7, fileSize);

        query.exec();
        qDebug() << "lastError:" << query.lastError().text();
    }
}

void TcpMsgServer::sltTransFileToClient(const int &userId, const QJsonValue &json)
{
    // 查找要发送过去的id
    for (int i = 0; i < clients.size(); i++) {
        if (userId == clients.at(i)->getUserID()){
            clients.at(i)->sltSendMessage(SendFile, json);
            return;
        }
    }
}

//-----------------------------------------


/*
 * 文件中转服务器，客户端先把待转发的文件发送到服务器，
 * 服务器接受完成后，通知其他客户端来下载
*/
TcpFileServer::TcpFileServer(QObject *parent) :
    TcpServer(parent)
{
}

TcpFileServer::~TcpFileServer()
{
    qDebug() << "tcp server close...";
    foreach (ClientFileSocket *client, clients) {
        clients.removeOne(client);
        client->close();
    }
}

void TcpFileServer::sltNewConnection()
{
    ClientFileSocket *client = new ClientFileSocket(this, tcpServer->nextPendingConnection());
    qDebug() << "new file socket connected,client info: " << client->getClientInfo() << endl;
    connect(client, SIGNAL(signalConnected()), this, SLOT(sltConnected()));
    connect(client, SIGNAL(signalDisConnected()), this, SLOT(sltDisConnected()));
    connect(client, SIGNAL(signalMsgToClient(const quint8 &, const int &, const QJsonValue &)),
            this, SIGNAL(signalMsgToClient(const quint8 &, const int &, const QJsonValue &)));

    connect(client, SIGNAL(signalDownloadFile(const QJsonValue&)),
            this, SLOT(sltClientDownloadFile(const QJsonValue&)));
}

void TcpFileServer::sltConnected()
{
    ClientFileSocket *client = static_cast<ClientFileSocket*>(this->sender());
    if(client == nullptr)
        return;

    clients.push_back(client);
}

void TcpFileServer::sltDisConnected()
{
    ClientFileSocket *client = static_cast<ClientFileSocket*>(this->sender());
    if (client == nullptr)
        return;

    for (int i = 0; i < clients.size(); i++) {
        if (client == clients.at(i))
        {
            clients.remove(i);
            return;
        }
    }

    disconnect(client, SIGNAL(signalConnected()), this, SLOT(sltConnected()));
    disconnect(client, SIGNAL(signalDisConnected()), this, SLOT(sltDisConnected()));
}

void TcpFileServer::sltClientDownloadFile(const QJsonValue &json)
{
    // 根据ID寻找连接的socket
    if (json.isObject()) {
        QJsonObject jsonObj = json.toObject();
        int tag = jsonObj.value("tag").toInt();

        qint32 sendID = jsonObj.value("from").toInt();
        qint32 recvID = jsonObj.value("to").toInt();
        qint32 groupID = jsonObj.value("group").toInt();
        qint64 time = jsonObj.value("time").toInt();
        int flag = jsonObj.value("flag").toInt();

        QString filename = DataBase::Instance()->getSentFile(sendID,recvID,time);

        if(tag != -2){
            qDebug() << recvID << "is getting file: " << filename
                     << (tag == 1 ? "[sent from group" + QString::number(groupID) +
                                    " by user" + QString::number(sendID) + "]"
                                  : "[sent from user" + QString::number(sendID)) + "]";
        }

        int nId = 0;
        int nWid = 0;
        if(tag == 0){
            nId = recvID;
            nWid = sendID;
            if(flag == -1){//用户接收离线消息文件
                nWid = -2;
            }
        }else if(tag == 1){
            nId = recvID;
            nWid = groupID;
            if(flag == -1){//用户接收离线消息文件
                nWid = -2;
            }
        }else if(tag == -2){
            //用户在获取头像
            int requestID = jsonObj.value("from").toInt();
            int targetID = jsonObj.value("who").toInt();

            nId = requestID;
            nWid = -2;
            filename = MyApp::m_strHeadPath + QString::number(targetID) + "/" + QString::number(targetID) + ".png";
            qDebug() << requestID << "is requesting head:" << targetID;
        }

        for (int i = 0; i < clients.size(); i++) {
            if (clients.at(i)->checkUserID(nId, nWid)){
                if(tag == 0){
                    clients.at(i)->startTransferFile(filename,0,time,flag);//私发时第二个参数无效
                }else if(tag == 1){
                    clients.at(i)->startTransferFile(filename,sendID,time,flag);//群发时第二个参数表示发送者
                }else if(tag == -2){
                    clients.at(i)->startTransferFile(filename,0,0,0);//群发时第二个参数表示发送者
                }

                return;
            }
        }
    }
}

