#ifndef CONTACTWIDGET_H
#define CONTACTWIDGET_H

#include "listwidget.h"
#include "mybutton.h"

#include <QWidget>
#include <QStackedWidget>
#include <QHash>

class ContactWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ContactWidget(QWidget *parent = nullptr);
    void InitList();
    void addCell(Cell*);
    void deleteFriend(int id);

private:
    ListWidget *friendList;
    ListWidget *groupList;
    MyButton *friendBtn;
    MyButton *groupBtn;
    QButtonGroup *btnGroup;
    QStackedWidget *stackWidget;

    QHash<QString,Cell*> hash;//通过分组名找到对应的cell抽屉
    Cell *popMenuCell = nullptr;

signals:
    void signalSendMessage(const quint8 &, const QJsonValue &);
    void openDialog(Cell*);
    void deleteChat(int id);

public slots:
    void onFriendDadMenuSelected(QAction*);
    void onGroupDadMenuSelected(QAction*);
    void onSonMenuSelected(QAction*);
    void onSonDoubleClicked(Cell*);//双击好友格子
    void onSwitchPage(int);
    void setPopMenuCell(Cell*,QMenu*);
    void sltUpdateFriendList(QString);
};

#endif // CONTACTWIDGET_H
