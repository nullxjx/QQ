#ifndef USERINFOWND_H
#define USERINFOWND_H

#include "mybutton.h"

#include <QWidget>
#include <QLabel>

class UserInfoWnd : public QWidget
{
    Q_OBJECT
public:
    explicit UserInfoWnd(QWidget *parent = nullptr,int id = 0,QString name = "",QString head = "",int sex = 0);

signals:

public slots:

private:
    QLabel *idLabel;
    QLabel *nameLabel;
    QLabel *headLabel;
    QLabel *sexLabel;
    QLabel *subGroupLabel;
    MyButton *sendMsgBtn;
    QLabel *schoolLabel;
    QLabel *regionLabel;
    QLabel *emailLabel;

    int id;
    QString name;
    QString head;
    int sex;

protected:
    void paintEvent(QPaintEvent*);
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);
    void showEvent(QShowEvent*);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
};

#endif // USERINFOWND_H
