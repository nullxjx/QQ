#ifndef ADDFRIENDWND_H
#define ADDFRIENDWND_H

#include "cell.h"
#include "roundlabel.h"

#include <QDialog>
#include <QLabel>
#include <QPushButton>

class AddFriendWnd : public QDialog
{
    Q_OBJECT

public:
    AddFriendWnd(Cell *cell);

signals:
    void signalAddFriend(const quint8 &,const QJsonValue&);
    void signalAddChat(Cell*);

public slots:
    void sltBtnClicked();

private:
    RoundLabel *headLabel;
    QLabel *idLabel;
    QLabel *nameLabel;
    QPushButton *agreeBtn;
    QPushButton *rejectBtn;
    QLabel *noticeLabel;
    QFont font;

    Cell *cell;
};

#endif // ADDFRIENDWND_H
