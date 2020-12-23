#ifndef DATABASE_H
#define DATABASE_H

#include "bubbleinfo.h"
#include "cell.h"

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMutex>


class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    bool openDb(const QString &dataName);

    // 单实例运行
    static DataBase *Instance()
    {
        static QMutex mutex;
        if (self == nullptr) {
            QMutexLocker locker(&mutex);

            if (!self) {
                self = new DataBase();
            }
        }
        return self;
    }

    // 添加历史聊天记录
    void addHistoryMsg(BubbleInfo *info);
    // 添加好友
    void addFriend(const int &myID, const int &friendID);
    // 添加群组
    void addGroup(const int &myID, const int &groupID);

    // 删除好友
    bool deleteMyFriend(const int &myID, const int &friendID);

    // 获取我的好友
    QJsonArray getMyFriends() const;
    // 获取我的群组
    QJsonArray getMyGroups() const;
    // 获取我的分组
    QJsonArray getMySubgroup() const;
    // 获取我的历史聊天列表
    QJsonArray getMyChatList() const;
    // 获取好友信息
    QJsonObject getFriendInfo(int id) const;
    // 获取群信息
    QJsonObject getGroupInfo(int id) const;

    // 判断改好友是否已经是我的好友了
    bool isMyFriend(const int &friendID);
    // 判断是否已经加入该群组了
    bool isInGroup(const int &groupID);
    // 判断是否是一个群的群主
    bool isAdmin(int id,int groupID);

    void updateFileMsg(BubbleInfo *info);

    // 获取历史聊天记录,id表示聊天窗口好友或者群的id,tag标记这是私聊还是群聊窗口,count表示最近的记录数
    QVector<BubbleInfo *> queryHistory(int id,int tag, int count);

    // 测试使用，打印数据库中的所有信息
    void queryAll();
signals:

public slots:

private:
    static DataBase *self;

    // 数据库管理
    QSqlDatabase userdb;
};


#endif // DATABASE_H
