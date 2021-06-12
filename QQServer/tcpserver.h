#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "clientsocket.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QVector>


class TcpServer : public QObject
{
    Q_OBJECT

public:
    explicit TcpServer(QObject *parent = nullptr);
    ~TcpServer();

    bool startListen(int port = 6666);
    void closeListen();

signals:
    void signalUserStatus(const QString &);

protected:
    QTcpServer *tcpServer;

public slots:
    virtual void sltNewConnection() = 0;
    virtual void sltConnected() = 0;
    virtual void sltDisConnected() = 0;
};


class TcpMsgServer : public TcpServer
{
    Q_OBJECT
public:
    explicit TcpMsgServer(QObject *parent = nullptr);
    ~TcpMsgServer();

signals:
    void signalDownloadFile(const QJsonValue &json);

private:
    // 客户端管理
    QVector <ClientSocket*> clients;

public slots:
    void sltTransFileToClient(const int &userId, const QJsonValue &json);

private slots:
    void sltNewConnection();
    void sltConnected();
    void sltDisConnected();
    void sltMsgToClient(const quint8 &type, const int &id, const QJsonValue &json);
};


class TcpFileServer : public TcpServer {
    Q_OBJECT

public :
    explicit TcpFileServer(QObject *parent = nullptr);
    ~TcpFileServer();

signals:
    void signalRecvFinished(int id, const QJsonValue &json);
    void signalMsgToClient(const quint8 &, const int &, const QJsonValue &);

private:
    // 客户端管理
    QVector <ClientFileSocket *> clients;

private slots:
    void sltNewConnection();
    void sltConnected();
    void sltDisConnected();
    void sltClientDownloadFile(const QJsonValue &json);
};


#endif // TCPSERVER_H
