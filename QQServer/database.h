#ifndef DATABASE_H
#define DATABASE_H

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
    void closeDb();

    // 单实例
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

    // 更新用户状态/上下线
    void updateUserStatus(const int &id, const quint8 &status);
    void updateUserHead(const int &id, const QString &strHead);
    void testHeadPic(const int &id, const QString &name, const QString &strHead);

    // 获取用户状态
    QJsonArray getAllUsers();
    QJsonObject getUserStatus(const int &id) const;
    int getUserLineStatus(const int &id) const;

    // 用户校验
    QJsonObject checkUserLogin(const int &id, const QString &passwd);
    // 注册用户
    int registerUser(QString name, QString pwd);

    // 添加好友
    QJsonObject addFriend(const QString &name);

    // 添加群组
    QJsonObject addGroup(const int &userId, const QString &name);
    // 创建群组
    QJsonObject createGroup(const int &adminID, const QString &name, qint64 time);
    // 查询当前群组下面的好友
    QJsonArray  getGroupUsers(const int &groupId);

    // 查询文件发送历史，获取要发送的文件在服务器上的完整地址
    QString getSentFile(int senderID,int receiverID,qint64 time);

    // 服务器启动的时候更新下所以人员的状态，可以不要
    void changeAllUserStatus();

    // 查询当前id的名字
    QString getUserName(const int &id) const;
    QString getUserHead(const int &id) const;
    QJsonObject getUserInfo(const int &id) const;
    QJsonObject getGroupInfo(const int &id) const;

    int getGroupAdmin(int groupid);

    QJsonObject changePwd(int id,QString oldpwd,QString newpwd);

    //获取离线消息
    QJsonArray getOfflineMsg(int id);

signals:

private:
    static DataBase *self;
    QSqlDatabase userdb;
    void queryAll();

public slots:
};

#endif // DATABASE_H
