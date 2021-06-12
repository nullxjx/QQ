#ifndef MAINUI_H
#define MAINUI_H

#include "leftbar.h"
#include "midbar.h"
#include "rightbar.h"
#include "clientsocket.h"
#include "settingswnd.h"

#include <QWidget>

class MainUI : public QWidget
{
    Q_OBJECT
public:
    explicit MainUI(QWidget *parent = nullptr);
    ~MainUI();
    void setSocket(ClientSocket *,ClientFileSocket*);
    void initUI();

private:
    LeftBar *leftBar;
    MidBar *midBar;
    RightBar *rightBar;
    ClientSocket *tcpSocket;

    ClientFileSocket *globalFileSocket;

    SettingsWnd *settings;
protected:
    void paintEvent(QPaintEvent*);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

signals:

public slots:
    void onBtnClicked(int);
    void stayOnTop(bool);
    void sltTcpReply(quint8,QJsonValue);
    void sltTcpStatus(quint8);
    void sltAddChat(Cell*);

    void parseFriendMessageReply(const QJsonValue &dataVal);
    void parseGetGroupMembersReply(const QJsonValue &dataVal);
    void parseGroupMessageReply(const QJsonValue &dataVal);
    void parseSendFileReply(const QJsonValue &dataVal);
    void parseAddFriendReply(const QJsonValue &dataVal);
    void parseDeleteFriendReply(const QJsonValue &dataVal);
    void parseAddGroupReply(const QJsonValue &dataVal);
    void parseDeleteGroupReply(const QJsonValue &dataVal);
    void parseAddFriendRequestConfirmed(const QJsonValue &dataVal);
    void parseAddGroupRequestConfirmed(const QJsonValue &dataVal);

    void sltFileRecvFinished(quint8,QString,int);
};

#endif // MAINUI_H
