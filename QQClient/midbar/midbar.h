#ifndef MIDBAR_H
#define MIDBAR_H

#include "contactwidget.h"
#include "listwidget.h"
#include "blankpage.h"
#include "creategroupwnd.h"
#include "addfriendwnd.h"

#include <QWidget>
#include <QStackedWidget>

class MidBar : public QWidget
{
    Q_OBJECT
public:
    explicit MidBar(QWidget *parent = nullptr);
    ~MidBar();
    void SltMainPageChanged(int);
    void InitChatList();
    void InitContactList();
    Cell* isIDExist(int id);
    void insertCell(Cell *c);
    void switchToChatList();
    void addCellToContact(Cell*);
    void deleteFriend(int id);
    Cell *getSelectedCell(){return selectedCell;}

protected:
    void paintEvent(QPaintEvent *);

private:
    QStackedWidget *mainWidget;
    ContactWidget *contactWidget;
    ListWidget *chatList;
    BlankPage *blankPage;

    CreateGroupWnd *newGroup;

    Cell *popMenuCell = nullptr;
    Cell *selectedCell = nullptr;//选中的格子，即如果右边有对话框时，对应中间的格子就是这个,初始化时没有被选中的

    QMenu *addMenu;

signals:
    void openDialog(Cell*);
    void signalSendMessage(const quint8 &, const QJsonValue &);
    void signalFindFriendReply(const QJsonValue &);
    void signalAddChat(Cell*);
    void resetRightPage();
    void contactBtnClicked();

public slots:
    void onSonMenuSelected(QAction*);
    void sltOpenDialog(Cell*);
    void setPopMenuCell(Cell*,QMenu*);

    void updateTime(int,qint64,QString);

    void sltMenuSelected(QAction*);

    void sltFind(const QJsonValue&);
    void sltCreateGroup(const QJsonValue &jsonVal);

    void sltAddFriend(Cell *cell);

    void deleteChatCell(int);
};

#endif // MIDBAR_H
