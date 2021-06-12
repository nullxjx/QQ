#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include "titlebar.h"
#include "chatwindow.h"
#include "clientsocket.h"
#include "defaultpage.h"

#include <QWidget>
#include <QByteArray>
#include <QStackedWidget>
#include <QList>
#include <QHash>

class RightBar : public QWidget
{
    Q_OBJECT

public:
    RightBar(QWidget *parent = nullptr);
    ~RightBar();

    QStackedWidget *stackWidget;
    ChatWindow *defaultWindow;
    void switchPage(Cell *);
    void msgconfirmed(QJsonValue dataVal);//这是服务器回的确认收到本机发出去的消息，
    void msgReceived(Cell *c,QJsonValue dataVal);//这是服务器转发给我的消息，是我的好友或者群发给我的消息
    void refreshGroupList(QJsonValue dataVal);
    void addNewUserToGroupList(int groupID,Cell *newUser);//新用户入群
    void removeUserFromGroupList(int groupID,int userID);//用户退群

    void resetPage();
    void restorePage();
    void setTitleText(QString text){this->titleBar->setTitleText(text);}
    void setCurrentPage(int page){currentPage = page;}
    void forbidSendMsg(int id);
    void allowSendMsg(int id);
    void startLoadingAnimation();
    void stopLoadingAnimation();

signals:
    void stayOnTop(bool);
    void signalSendMessage(const quint8 &, const QJsonValue &);
    void updateMidBarTime(int id,qint64 time,QString msg);

private:
    TitleBar *titleBar;

    DefaultPage *defaultPage;
    QList<ChatWindow*> chatWindowList;
    QHash<int,int> hash;//用户对应id到聊天窗口的映射
    int cnt = 1;
    int currentPage = 0;
    ClientSocket *tcpSocket;

    QLabel *loadingLabel;
    QMovie *loadingMovie;

public slots:

};

#endif // BASEWIDGET_H
