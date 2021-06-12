/****************************************************************************
** QQ-Like 登录界面窗体
****************************************************************************/

#ifndef LOGINWND_H
#define LOGINWND_H

#include "loginwndmainpage.h"
#include "loginwndloginingpage.h"
#include "clientsocket.h"

#include <QStackedWidget>


class LoginWnd : public QStackedWidget
{
    Q_OBJECT

public:
    explicit LoginWnd(QWidget *parent = nullptr);
    void showMainUI();

signals:
    void rotateWindow();
    void closeWindow();
    void hideWindow();
    void loginSuccess();
    void openRegisterWnd();
    void openChangePasswordWnd();

    void signalRegisterOK(const QJsonValue&);
    void signalChangePwdReply(const QJsonValue &dataVal);

public slots:
    void changePage();
    void showNotifyMsg(QString);
    void hideNotifyMsg();
    void sltTcpStatus(const quint8 &state);
    void slotTimeout();

    void sltFileRecvFinished(quint8,QString,int);
    void checkAutoLogin();

    void sltRegister(const QJsonValue &);
    void sltChangePwd(const QJsonValue &);

    void writeOffLineMsgToDatabase(const QJsonValue &);
    void sltGetOffLineMsg();

private:
    LoginWndMainPage *loginMainPage;
    LoginWndLoginingPage *loginingPage;
    ClientSocket *tcpSocket;
    bool connected;
    QTimer *timer;

    ClientFileSocket *globalFileSocket;

    QLabel *m_notifyMsg;// 提醒信息

protected:
    void paintEvent(QPaintEvent*);
};

#endif // LOGINWND_H
