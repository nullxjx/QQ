#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>


class ClientSocket : public QObject
{
    Q_OBJECT

public:
    explicit ClientSocket(QObject *parent = nullptr);
    ~ClientSocket();

    // 获取当前用户的ID
    int getUserID() const;
    void setUserID(const int &id);

    void checkConnected();
    void closeConnection();

    // 连接服务器
    void connectToHost(const QString &host, const int &port);
    void connectToHost(const QHostAddress &host, const int &port);

signals:
    void signalMessage(const quint8 &type, const QJsonValue &dataVal);
    void signalStatus(const quint8 &state);
    void signalRegisterOk(const QJsonValue &dataVal);
    void signalChangePwdReply(const QJsonValue &dataVal);
    void signalFindFriendReply(const QJsonValue &dataVal);
    void signalGetOfflineMsg(const QJsonValue &dataVal);

public slots:
    // socket消息发送封装
    void sltSendMessage(const quint8 &type, const QJsonValue &dataVal);
    // 发送上线通知
    void sltSendOnline();
    // 发送下线通知
    void sltSendOffline();

private:
    // tcpsocket
    QTcpSocket *tcpSocket;
    int ID;

private slots:
    // 与服务器断开链接
    void sltDisconnected();
    // 链接上服务器
    void sltConnected();
    // tcp消息处理
    void sltReadyRead();

private:
    // 解析登陆返回信息
    void parseLogin(const QJsonValue &dataVal);
    // 解析注册返回信息
    void parseReister(const QJsonValue &dataVal);
};



class ClientFileSocket : public QObject
{
    Q_OBJECT

public:
    explicit ClientFileSocket(QObject *parent = nullptr);
    ~ClientFileSocket();

    bool isConneciton();
    void setTag(int tag){this->tag = tag;}
    bool isBusy(){return this->busy;}

    // 发送文件大小等信息
    void startTransferFile(QString fileReadName,qint64 time,quint8 type);

    // 连接到服务器
    void connectToServer(const QString &ip, const int &port, const int &usrId);
    // 断开服务器
    void closeConnection();

    // 文件传输完成
    void fileTransFinished();

    // 设置当前socket的id
    void setUserId(const int &id);

signals:
    void signalSendFinished(quint8,QString);
    void signalFileRecvOk(const quint8 &type, const QString &filePath,int senderID);
    void signalUpdateProgress(qint64 currSize, qint64 total);
    void signalConnectd();

    void signalFileArrived(const QJsonValue&);

private:
    quint64         loadSize;   //每次发送数据的大小

    /************* Receive file *******************/
    qint64          bytesReceived;      //已收到数据的大小
    quint64         fileNameSize;       //文件名的大小信息
    QString         fileReadName;       //存放文件名
    QByteArray      inBlock;            //数据缓冲区
    qint64          ullRecvTotalBytes;  //数据总大小
    QFile           *fileToRecv;        //要发送的文件
    qint64          senderID;           //发送文件者id
    qint64          RecvFileSendTime;   //文件发送时间

    /************* Send file **********************/
    quint16         blockSize;          //存放接收到的信息大小
    QFile           *fileToSend;        //要发送的文件
    qint64          ullSendTotalBytes;  //数据总大小
    qint64          bytesWritten;       //已经发送数据大小
    quint64         bytesToWrite;       //剩余数据大小
    QByteArray      outBlock;           //数据缓冲区，即存放每次要发送的数据
    QString         fileSendName;       //存放发送文件的名字(包括路径)

    // 用户目录
    QString         strFilePath;

    // 通信类
    QTcpSocket      *tcpSocket;

    bool            busy;
    int             winID;
    quint8          type;

    int tag = 0;//tag = 0表示这是私聊窗口的一个filesocket,tag = 1表示这是群聊窗口的一个filesocket

    qint64 flag;

private:
    // socket 初始化
    void initSocket();

public slots:

private slots:
    // 显示错误
    void displayError(QAbstractSocket::SocketError);
    // 发送文件数据，更新进度条
    void sltUpdateClientProgress(qint64);

    // 文件接收
    void sltReadyRead();
    void sltConnected();
    void sltDisConnected();
};


#endif // TCPCLIENT_H
