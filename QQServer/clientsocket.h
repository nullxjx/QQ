#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QApplication>
#include <QHostAddress>
#include <QDateTime>


class ClientSocket : public QObject
{
    Q_OBJECT
public:
    explicit ClientSocket(QObject *parent = nullptr, QTcpSocket *tcpSocket = nullptr);
    ~ClientSocket();

    int getUserID() const;
    void close();
    QString getClientInfo(){
        return QString("") + "ip:" + QHostAddress( tcpSocket->peerAddress().toIPv4Address() ).toString()
                + " port:" + QString::number(tcpSocket->peerPort()) ;
    }

signals:
    void signalConnected();
    void signalDisConnected();
    void signalDownloadFile(const QJsonValue &json);
    void signalMsgToClient(const quint8 &type, const int &reveicerID, const QJsonValue &dataVal);

private:
    QTcpSocket *tcpSocket;
    int         ID;

public slots:
    //消息回发
    void sltSendMessage(const quint8 &type, const QJsonValue &json);

private slots:
    void sltConnected();
    void sltDisconnected();
    void sltReadyRead();

private:
    // 消息解析和转发处理
    void parseLogin(const QJsonValue &dataVal);
    void parseLogout(const QJsonValue &dataVal);
    void parseUserOnline(const QJsonValue &dataVal);
    void parseUpdateUserHead(const QJsonValue &dataVal);

    void parseReister(const QJsonValue &dataVal);

    void parseAddFriend(const QJsonValue &dataVal);
    void parseDeleteFriend(const QJsonValue &dataVal);
    void parseAddFriendReply(const QJsonValue &dataVal);

    void parseAddGroup(const QJsonValue &dataVal);
    void parseDeleteGroup(const QJsonValue &dataVal);
    void parseAddGroupReply(const QJsonValue &dataVal);
    void parseCreateGroup(const QJsonValue &dataVal);

    void parseGetMyFriend(const QJsonValue &dataVal);
    void parseGetMyGroups(const QJsonValue &dataVal);
    void parseGetGroupMembers(const QJsonValue &dataVal);

    void parseRefreshFriend(const QJsonValue &dataVal);
    void parseRefreshGroups(const QJsonValue &dataVal);

    void parseFriendMessages(const QByteArray &reply);
    void parseGroupMessages(const QByteArray &reply);

    void parseSendPicture(const QJsonValue &dataVal);
    void parseChangePwd(const QJsonValue &dataVal);
    void parseFindFriend(const QJsonValue &dataVal);

    void parseGetOfflineMsg(const QJsonValue &dataVal);
};


//--------------------------------------------


class ClientFileSocket : public QObject
{
    Q_OBJECT

public:
    explicit ClientFileSocket(QObject *parent = nullptr, QTcpSocket *tcpSocket = nullptr);
    ~ClientFileSocket();

    void close();
    bool checkUserID(const qint32 nId, const qint32 &winId);
    QString getClientInfo(){
        return QString("") + "ip:" + QHostAddress( tcpSocket->peerAddress().toIPv4Address() ).toString()
                + " port:" + QString::number(tcpSocket->peerPort()) ;
    }

    // 文件传输完成
    void fileTransFinished();
    void startTransferFile(QString fileName,int senderID, qint64 time,int flag = 0);
    void insertDataBase(QString filepath,QString filename);

signals:
    void signalConnected();
    void signalDisConnected();
    void signalRecvFinished(int id, const QJsonValue &json);
    void signalMsgToClient(const quint8 &type, const int &reveicerID, const QJsonValue &dataVal);

    void signalDownloadFile(const QJsonValue &json);

private:
    /************* Receive file *******************/
    quint64 loadSize;
    quint64 bytesReceived;      //已收到数据的大小
    quint64 fileNameSize;       //文件名的大小信息
    QString fileReadName;       //存放文件名
    QString filesavepath;       //存放文件的保存路径
    QByteArray inBlock;         //数据缓冲区
    quint64 ullRecvTotalBytes;  //数据总大小
    QFile *fileToRecv;          //要发送的文件

    QTcpSocket *tcpSocket;

    /************* send file *******************/
    quint16 blockSize;          //存放接收到的信息大小
    QFile *fileToSend;          //要发送的文件
    quint64 ullSendTotalBytes;  //数据总大小
    quint64 bytesWritten;       //已经发送数据大小
    quint64 bytesToWrite;       //剩余数据大小
    QByteArray outBlock;        //数据缓冲区，即存放每次要发送的数据


    bool busy;

    // 需要转发的用户id,即发送文件的用户ID
    qint32 userID;
    // 当前用户的窗口好友的id,即需要接收文件的用户ID
    qint32 windowID;

    qint32 tag;//标记聊天双方是私聊还是群聊，tag=0表示私聊，tag=1表示群聊
    qint64 msgSendTime;

private:
    void initSocket();

public slots:

private slots:
    void displayError(QAbstractSocket::SocketError); // 显示错误
    // 文件接收
    void sltReadyRead();
    // 发送
    void sltUpdateClientProgress(qint64 numBytes);
};


#endif // CLIENTSOCKET_H
