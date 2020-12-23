#ifndef MAINUI_H
#define MAINUI_H

#include "tcpserver.h"
#include "database.h"
#include "myapp.h"
#include "global.h"
#include "unit.h"

#include <QWidget>

class MainUI : public QWidget
{
    Q_OBJECT

public:
    MainUI(QWidget *parent = nullptr);
    ~MainUI();

private:
    TcpMsgServer *tcpMsgServer;
    TcpFileServer *tcpFileServer;

    void InitNetwork();

public slots:
    void showUserStatus(QString);
};

#endif // MAINUI_H
