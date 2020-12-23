#ifndef CELL_H
#define CELL_H

#include <QString>
#include <QList>

typedef enum CellType
{
    Cell_FriendChat = 0,    //聊天列表中好友小格子
    Cell_GroupChat = 1,     //聊天列表中群小格子
    Cell_FriendContact = 2, //好友的一个小格子
    Cell_GroupContact = 3,  //群的一个小格子
    Cell_FriendDrawer = 4,  //好友列表的一级标题
    Cell_GroupDrawer = 5,   //群列表的一级标题
    Cell_AddFriend = 6,     //请求添加好友
    Cell_AddGroup = 7       //请求加群
} CellType;


class Cell
{
public:
    Cell();
    void SetSubtitle(const QString &text);
    void SetIconPath(const QString &path);
    void SetStatus(const quint8 &val);
    void sortById();
    void sortByName();

public:
    QString iconPath;
    QString name;
    QString subTitle;
    QString msg = "";

    QString ipaddr;
    QString groupName;      //组名
    int     id;             //QQ号，作为标识
    int     status;
    CellType type;

    //Cell_AddGroup专用
    int groupid;
    QString groupname_;
    QString groupHead;

    int newUserID;
    QString newUserName;
    QString newUserHead;

    bool done = false;
    bool deleted = false;
    bool stayOnTop = false;

    //下面几项都是只用于Cell_FriendDrawer = 2 和 Cell_GroupDrawer = 3
    bool isOpen;            //标记下拉抽屉的打开状态
    bool isClicked;
    bool showNewMsg = false;
    QList<Cell *> childs;   //子项
};

#endif // CELL_H
