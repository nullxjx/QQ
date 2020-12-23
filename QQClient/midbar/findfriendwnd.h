#ifndef FINDFRIENDWND_H
#define FINDFRIENDWND_H

#include "searchbar.h"
#include "roundlabel.h"

#include <QDialog>
#include <QWidget>
#include <QLabel>

class FindFriendWnd : public QDialog
{
    Q_OBJECT

public:
    FindFriendWnd(int tag = 0);

signals:
    void signalFind(const QJsonValue&);
    void signalSendMessage(const quint8 &, const QJsonValue &);

public slots:
    void sltBtnClicked();
    void sltFindFriendReply(const QJsonValue &);
    void sltAddFriend();

private:
    SearchBar *searchBar;
    QLabel *resultLabel;
    RoundLabel *headLabel;
    QLabel *idLabel;
    QLabel *nameLabel;
    QLabel *msgLabel;

    QPushButton *addBtn;

    int tag;
    int friendID;
};

#endif // FINDFRIENDWND_H
