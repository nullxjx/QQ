#include "mainui.h"

#include <QDebug>

MainUI::MainUI(QWidget *parent)
    : QWidget(parent)
{
    InitNetwork();
}

MainUI::~MainUI()
{

}

void MainUI::InitNetwork()
{
    tcpMsgServer = new TcpMsgServer(this);
    bool ok = tcpMsgServer->startListen(60100);
    if(ok){
        qDebug() << "消息服务器监听成功,端口: 60100";
    }else{
        qDebug() << "消息服务器监听失败";
    }

    tcpFileServer = new TcpFileServer(this);
    ok = tcpFileServer->startListen(60101);
    if(ok){
        qDebug() << "文件服务器监听成功,端口: 60101";
    }else{
        qDebug() << "文件服务器监听失败";
    }

    qDebug() << "--------------------------------------------------------------------" << endl;

    connect(tcpMsgServer, SIGNAL(signalDownloadFile(QJsonValue)), tcpFileServer, SLOT(sltClientDownloadFile(QJsonValue)));
    connect(tcpMsgServer, SIGNAL(signalUserStatus(QString)), this, SLOT(showUserStatus(QString)));

    connect(tcpFileServer, SIGNAL(signalUserStatus(QString)), this, SLOT(showUserStatus(QString)));
    connect(tcpFileServer,SIGNAL(signalMsgToClient(const quint8 &, const int &, const QJsonValue &)),
            tcpMsgServer,SLOT(sltMsgToClient(quint8,int,QJsonValue)));
}

void MainUI::showUserStatus(QString str)
{
    qDebug() << str << endl
             << "......................................................" << endl;
}
