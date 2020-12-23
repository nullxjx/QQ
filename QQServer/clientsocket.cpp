#include "clientsocket.h"
#include "database.h"
#include "unit.h"
#include "myapp.h"
#include "global.h"

#include <QDebug>
#include <QDataStream>
#include <QApplication>
#include <QFileInfo>
#include <QBuffer>
#include <QPixmap>
#include <QDateTime>
#include <QSqlError>

ClientSocket::ClientSocket(QObject *parent, QTcpSocket *socket) : QObject(parent)
{
    ID = -1;

    if (socket == nullptr)
        tcpSocket = new QTcpSocket(this);
    else
        tcpSocket = socket;

    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(sltReadyRead()));
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(sltConnected()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(sltDisconnected()));
}

ClientSocket::~ClientSocket()
{

}

int ClientSocket::getUserID() const
{
    return ID;
}

void ClientSocket::close()
{
    tcpSocket->abort();
}

void ClientSocket::sltConnected()
{
    qDebug() << "new socket connected";
}

void ClientSocket::sltDisconnected()
{
    qDebug() << "socket disconnected";
    DataBase::Instance()->updateUserStatus(ID, OffLine);
    Q_EMIT signalDisConnected();
}

void ClientSocket::sltReadyRead()
{
    // 读取socket数据
    QByteArray reply = tcpSocket->readAll();
    QJsonParseError jsonError;
    // 转化为 JSON 文档
    QJsonDocument doucment = QJsonDocument::fromJson(reply, &jsonError);
    // 解析未发生错误
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {
        // JSON 文档为对象
        if (doucment.isObject()) {
            // 转化为对象
            QJsonObject jsonObj = doucment.object();
            int msgType = jsonObj.value("type").toInt();//提取消息类型并进行相应的消息解析
            int senderID = jsonObj.value("from").toInt();
            QJsonValue dataVal = jsonObj.value("data");

            //            qDebug() << "cur time:" << QDateTime::currentDateTime().
            //                        toString("yyyy-MM-dd  hh:mm:ss.zzz");
            qDebug() << "<- "  << "服务器收到一条来自于" <<  senderID
                     << "的消息，消息内容为:" << endl
                     << jsonObj << endl;

            switch (msgType) {
            case Register:
            {
                parseReister(dataVal);
            }
                break;
            case Login:
            {
                parseLogin(dataVal);
            }
                break;
            case UserOnLine:
            {
                parseUserOnline(dataVal);
            }
                break;
            case Logout:
            {
                parseLogout(dataVal);
                Q_EMIT signalDisConnected();
                tcpSocket->abort();
            }
                break;
            case UpdateHeadPic:
            {
                parseUpdateUserHead(dataVal);
            }
                break;
            case AddFriend:
            {
                parseAddFriend(dataVal);
            }
                break;
            case AddGroup:
            {
                parseAddGroup(dataVal);
            }
                break;
            case CreateGroup:
            {
                parseCreateGroup(dataVal);
            }
                break;
            case GetMyFriends:
            {
                parseGetMyFriend(dataVal);
            }
                break;
            case GetMyGroups:
            {
                parseGetMyGroups(dataVal);
            }
                break;
            case GetGroupMembers:
            {
                parseGetGroupMembers(dataVal);
            }
                break;

            case RefreshFriends:
            {
                parseRefreshFriend(dataVal);
            }
                break;
            case RefreshGroups:
            {
                parseRefreshGroups(dataVal);
            }
                break;

            case SendMsg://私发消息
            case SendFile:
            case SendPicture:
            {
                parseFriendMessages(reply);
            }
                break;
            case SendGroupMsg://群发消息
            {
                parseGroupMessages(reply);
            }
                break;
            case SendFace:
            {
                //parseGroupMessages(reply);
            }
                break;
            case SendFileOk:
            {

            }
                break;
            case GetFile:
            case GetPicture:
            {
                Q_EMIT signalDownloadFile(dataVal);
            }
                break;
            case ChangePasswd:
            {
                parseChangePwd(dataVal);
            }
                break;
            case FindFriend:
            {
                parseFindFriend(dataVal);
            }
                break;
            case AddFriendRequist:
            {
                parseAddFriendReply(dataVal);
            }
                break;
            case AddGroupRequist:
            {
                parseAddGroupReply(dataVal);
            }
                break;
            case DeleteFriend:
            {
                parseDeleteFriend(dataVal);
            }
                break;
            case DeleteGroup:
            {
                parseDeleteGroup(dataVal);
            }
                break;
            case GetOfflineMsg:
            {
                parseGetOfflineMsg(dataVal);
            }
                break;
            default:
                break;
            }
        }
    }
}

void ClientSocket::parseLogin(const QJsonValue &dataVal)
{
    // data 的 value 是对象
    if (dataVal.isObject()) {
        QJsonObject dataObj = dataVal.toObject();
        int id = dataObj.value("id").toInt();
        QString strPwd = dataObj.value("passwd").toString();
        //查询数据库验证用户的ID和密码是否匹配
        QJsonObject json = DataBase::Instance()->checkUserLogin(id, strPwd);

        int code = json.value("code").toInt();

        //登陆成功
        if(code == 0){
            ID = id;
            Q_EMIT signalConnected();
        }
        //把登陆消息返回给客户端
        sltSendMessage(Login,json);
    }
}

void ClientSocket::parseLogout(const QJsonValue &dataVal)
{
    // data 的 value 是对象
    if (dataVal.isObject()) {
        QJsonObject dataObj = dataVal.toObject();

        QJsonArray array = dataObj.value("friends").toArray();
        int nId = dataObj.value("id").toInt();
        int nSize = array.size();

        DataBase::Instance()->updateUserStatus(nId, OffLine);

        for (int i = 0; i < nSize; ++i) {
            nId = array.at(i).toInt();
            int nStatus = DataBase::Instance()->getUserLineStatus(nId);
            // 给在线的好友通报一下状态
            if (OnLine == nStatus) {
                QJsonObject jsonObj;
                jsonObj.insert("id", ID);
                jsonObj.insert("text", "offline");

                Q_EMIT signalMsgToClient(UserOffLine, nId, jsonObj);
            }
        }
    }
}

void ClientSocket::parseUserOnline(const QJsonValue &dataVal)
{
    // data 的 value 是数组
    if (dataVal.isArray()) {
        QJsonArray jsonArray = dataVal.toArray();
        int nSize = jsonArray.size();
        for (int i = 0; i < nSize; ++i) {
            int nId = jsonArray.at(i).toInt();
            int nStatus = DataBase::Instance()->getUserLineStatus(nId);
            // 给在线的好友通报一下状态
            if (OnLine == nStatus) {
                QJsonObject jsonObj;
                jsonObj.insert("id", ID);
                jsonObj.insert("text", "online");

                Q_EMIT signalMsgToClient(UserOnLine, nId, jsonObj);
            }
        }
    }
}

void ClientSocket::parseUpdateUserHead(const QJsonValue &dataVal)
{
    // data 的 value 是对象
    if (dataVal.isObject()) {
        QJsonObject dataObj = dataVal.toObject();
        int nId = dataObj.value("id").toInt();
        QString strHead = dataObj.value("head").toString();

        // 更新数据库
        DataBase::Instance()->updateUserHead(nId, strHead);

        // 通知其他在线好友，说我已经修改了头像
        QJsonArray jsonFriends =  dataObj.value("friends").toArray();
        QJsonObject jsonObj;
        // 是我在更新，我要去下载我的头像
        jsonObj.insert("id", nId);
        jsonObj.insert("head", strHead);

        for (int i = 0; i < jsonFriends.size(); i++) {
            nId = jsonFriends.at(i).toInt();
            Q_EMIT signalMsgToClient(UpdateHeadPic, nId, jsonObj);
        }
    }
}

void ClientSocket::parseReister(const QJsonValue &dataVal)
{
    // data 的 value 是对象
    if (dataVal.isObject()) {
        QJsonObject dataObj = dataVal.toObject();
        QString name = dataObj.value("name").toString();
        QString pwd = dataObj.value("pwd").toString();

        int id = DataBase::Instance()->registerUser(name, pwd);

        // 返回客户端
        QJsonObject json;
        json.insert("id", id);
        json.insert("pwd", pwd);
        json.insert("name",name);

        // 发送查询结果至客户端
        sltSendMessage(Register, json);
    }
}

void ClientSocket::parseAddFriend(const QJsonValue &dataVal)
{
    // data 的 value 是对象
    if (dataVal.isObject()) {
        QJsonObject json = dataVal.toObject();

        int id = json.value("id").toInt();
        int senderid = json.value("sender").toInt();

        QJsonObject jsonObj = DataBase::Instance()->getUserInfo(senderid);
        jsonObj.insert("msg","add friend request");
        jsonObj.insert("time",QDateTime::currentSecsSinceEpoch());
        jsonObj.insert("tag",0);

        Q_EMIT signalMsgToClient(AddFriend, id, jsonObj);
    }
}

void ClientSocket::parseDeleteFriend(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();
        int userID1 = json.value("userID1").toInt();
        int userID2 = json.value("userID2").toInt();

        QSqlQuery query;
        QString sql = "delete from Friendship where userID1=";
        sql.append(QString::number(userID1));
        sql.append(" and userID2=");
        sql.append(QString::number(userID2));
        query.exec(sql);

        QString sql2 = "delete from Friendship where userID1=";
        sql2.append(QString::number(userID2));
        sql2.append(" and userID2=");
        sql2.append(QString::number(userID1));
        query.exec(sql2);

        qDebug() << userID1 << "和" << userID2 << "解除好友关系" << endl;

        QJsonObject jsonObj;
        jsonObj.insert("msg","user" + QString::number(userID1) + " have deleted you.");
        jsonObj.insert("id",userID1);

        //通知被删除的用户
        emit signalMsgToClient(DeleteFriend,userID2,jsonObj);
    }
}

void ClientSocket::parseAddFriendReply(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();
        int userID1 = json.value("userID1").toInt();//发送请求者
        int userID2 = json.value("userID2").toInt();
        QString time = json.value("time").toString();
        //先更新数据库，再通知发送请求的用户

        QSqlQuery query;

        query.prepare("INSERT INTO Friendship (userID1, userID2, createTime) "
                      "VALUES (?, ?, ?);");
        query.bindValue(0, userID1);
        query.bindValue(1, userID2);
        query.bindValue(2, time);

        query.exec();

        //通知用户
        QJsonObject jsonObj = DataBase::Instance()->getUserInfo(userID2);
        Q_EMIT signalMsgToClient(AddFriendRequist, userID1, jsonObj);
    }
}

void ClientSocket::parseAddGroup(const QJsonValue &dataVal)
{
    // data 的 value 是对象
    if (dataVal.isObject()) {
        QJsonObject json = dataVal.toObject();
        int id = json.value("id").toInt();
        int senderid = json.value("sender").toInt();

        QJsonObject jsonObj = DataBase::Instance()->getUserInfo(senderid);
        QJsonObject groupInfo = DataBase::Instance()->getGroupInfo(id);
        int adminID = groupInfo.value("adminID").toInt();
        QString groupName = groupInfo.value("name").toString();
        QString groupHead = groupInfo.value("head").toString();

        if(adminID != -1){
            jsonObj.insert("msg","add group request");
            jsonObj.insert("time",QDateTime::currentSecsSinceEpoch());
            jsonObj.insert("group",id);
            jsonObj.insert("groupName",groupName);
            jsonObj.insert("groupHead",groupHead);
            jsonObj.insert("tag",1);

            // 发送查询结果至客户端
            // 通知该群的群主处理该条请求消息
            Q_EMIT signalMsgToClient(AddGroup, adminID, jsonObj);
        }
    }
}

void ClientSocket::parseDeleteGroup(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();
        int userID = json.value("id").toInt();
        int groupID = json.value("group").toInt();

        qDebug() << userID << "请求退出群" << groupID << endl;
        QSqlQuery query;
        QString sql = "delete from GroupUser where groupID=";
        sql.append(QString::number(groupID));
        sql.append(" and userID=");
        sql.append(QString::number(userID));
        query.exec(sql);


        //----------------------------------------------

        //通知该群的用户，更新聊天窗口中的群列表信息，并且显示该用户已经加入群
        QJsonObject newUserInfo = DataBase::Instance()->getUserInfo(userID);
        QString name = newUserInfo.value("name").toString();
        QString head = newUserInfo.value("head").toString();

        QJsonObject groupInfo = DataBase::Instance()->getGroupInfo(groupID);
        QString groupName = groupInfo.value("name").toString();

        QJsonArray jsonArr = DataBase::Instance()->getGroupUsers(groupID);
        for (int i = 0; i < jsonArr.size(); i++) {
            QJsonObject json = jsonArr.at(i).toObject();
            int id = json.value("id").toInt();

            if(id == userID)//不通知请求发送者,因为他已经处理了该条消息
                continue;

            // 重组消息
            QJsonObject jsonMsg;
            jsonMsg.insert("group", groupID);
            jsonMsg.insert("groupName",groupName);
            jsonMsg.insert("id", userID);//退群用户的id
            jsonMsg.insert("name", name);//退群用户的名字
            jsonMsg.insert("head",head);
            jsonMsg.insert("to", id);
            jsonMsg.insert("msg", QString::number(userID) + "已退出该群");
            jsonMsg.insert("tag",1);
            jsonMsg.insert("type",Notice);
            jsonMsg.insert("noticeType",ExitGroup);

            Q_EMIT signalMsgToClient(DeleteGroup, id, jsonMsg);
        }
    }
}

void ClientSocket::parseAddGroupReply(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();
        int userID = json.value("userID").toInt();//发送请求者
        int groupID = json.value("groupID").toInt();
        QString time = json.value("time").toString();
        //先更新数据库，再通知发送请求的用户

        QSqlQuery query;

        query.prepare("INSERT INTO GroupUser (groupID, userID, joinTime) "
                      "VALUES (?, ?, ?);");
        query.bindValue(0, groupID);
        query.bindValue(1, userID);
        query.bindValue(2, time);

        query.exec();

        //通知发送请求的用户
        QJsonObject jsonObj = DataBase::Instance()->getGroupInfo(groupID);
        Q_EMIT signalMsgToClient(AddGroupRequist, userID, jsonObj);

        //----------------------------------------------

        //通知该群的用户，更新聊天窗口中的群列表信息，并且显示该用户已经加入群
        QJsonObject newUserInfo = DataBase::Instance()->getUserInfo(userID);
        QString name = newUserInfo.value("name").toString();
        QString head = newUserInfo.value("head").toString();

        QJsonObject groupInfo = DataBase::Instance()->getGroupInfo(groupID);
        QString groupName = groupInfo.value("name").toString();
        int adminID = groupInfo.value("adminID").toInt();

        QJsonArray jsonArr = DataBase::Instance()->getGroupUsers(groupID);
        for (int i = 0; i < jsonArr.size(); i++) {
            QJsonObject json = jsonArr.at(i).toObject();
            int id = json.value("id").toInt();

            if(id == userID || id == adminID)//不通知请求发送者和群主，因为他们已经处理了该条消息
                continue;

            // 重组消息
            QJsonObject jsonMsg;
            jsonMsg.insert("group", groupID);
            jsonMsg.insert("groupName",groupName);
            jsonMsg.insert("id", userID);//新入群用户的id
            jsonMsg.insert("name", name);//新入群用户的名字
            jsonMsg.insert("head",head);
            jsonMsg.insert("time",time);
            jsonMsg.insert("to", id);
            jsonMsg.insert("msg", QString::number(userID) + "已加入该群");
            jsonMsg.insert("tag",1);
            jsonMsg.insert("type",Notice);
            jsonMsg.insert("noticeType",NewMember);

            Q_EMIT signalMsgToClient(SendGroupMsg, id, jsonMsg);
        }
    }
}

void ClientSocket::parseCreateGroup(const QJsonValue &dataVal)
{
    // data 的 value 是对象
    if (dataVal.isObject()) {
        QJsonObject dataObj = dataVal.toObject();
        QString name = dataObj.value("name").toString();
        int adminID = dataObj.value("adminID").toInt();
        qint64 time = dataObj.value("time").toInt();

        QJsonObject json = DataBase::Instance()->createGroup(adminID,name,time);
        // 发送查询结果至客户端
        sltSendMessage(CreateGroup, json);
    }
}

void ClientSocket::parseGetMyFriend(const QJsonValue &dataVal)
{
    QJsonArray jsonArray;
    // data 的 value 是数组
    if (dataVal.isArray()) {
        QJsonArray array = dataVal.toArray();
        int nSize = array.size();
        // 先添加在线好友，在添加离线好友
        for (int i = 0; i < nSize; ++i) {
            int nId = array.at(i).toInt();
            jsonArray.append(DataBase::Instance()->getUserStatus(nId));
        }
    }

    sltSendMessage(GetMyFriends, jsonArray);
}

void ClientSocket::parseGetMyGroups(const QJsonValue &dataVal)
{
    QJsonArray jsonArray;
    // data 的 value 是数组
    if (dataVal.isObject()) {
        QJsonObject jsonObj = dataVal.toObject();
        // 群组ID
        int nId = jsonObj.value("id").toInt();
        jsonArray = DataBase::Instance()->getGroupUsers(nId);
    }

    sltSendMessage(GetMyGroups, jsonArray);
}

void ClientSocket::parseGetGroupMembers(const QJsonValue &dataVal)
{
    if (dataVal.isObject()) {
        QJsonObject jsonObj = dataVal.toObject();
        // 群组ID
        int nId = jsonObj.value("id").toInt();
        int groupID = jsonObj.value("groupid").toInt();
        qDebug() << nId << "请求获取" << groupID << "的成员";

        QJsonArray jsonArray = DataBase::Instance()->getGroupUsers(groupID);
        sltSendMessage(GetGroupMembers,jsonArray);
    }
}

void ClientSocket::parseRefreshFriend(const QJsonValue &dataVal)
{
    QJsonArray jsonArray;
    // data 的 value 是数组
    if (dataVal.isArray()) {
        QJsonArray array = dataVal.toArray();
        int nSize = array.size();
        for (int i = 0; i < nSize; ++i) {
            int nId = array.at(i).toInt();
            jsonArray.append(DataBase::Instance()->getUserStatus(nId));
        }
    }

    sltSendMessage(RefreshFriends, jsonArray);
}

void ClientSocket::parseRefreshGroups(const QJsonValue &dataVal)
{
    QJsonArray jsonArray;
    // data 的 value 是数组
    if (dataVal.isObject()) {
        QJsonObject jsonObj = dataVal.toObject();
        // 群组ID
        int nId = jsonObj.value("id").toInt();
        jsonArray = DataBase::Instance()->getGroupUsers(nId);
    }

    sltSendMessage(RefreshGroups, jsonArray);
}

void ClientSocket::parseFriendMessages(const QByteArray &reply)
{
    // 重新组装数据
    QJsonParseError jsonError;
    // 转化为 JSON 文档
    QJsonDocument doucment = QJsonDocument::fromJson(reply, &jsonError);
    // 解析未发生错误
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {
        // JSON 文档为对象
        if (doucment.isObject()) {
            // 转化为对象
            QJsonObject jsonObj = doucment.object();
            int nType = jsonObj.value("type").toInt();
            QJsonValue dataVal = jsonObj.value("data");

            //服务器收到了用户发来的消息，向用户回复已收到
            sltSendMessage(MsgReceived,dataVal);

            QJsonObject dataObj = dataVal.toObject();
            int reveicerID = dataObj.value("to").toInt();//获取消息接受者的ID

            Q_EMIT signalMsgToClient(quint8(nType), reveicerID, dataObj);//(type,reveicerID,data)
        }
    }
}

void ClientSocket::parseGroupMessages(const QByteArray &reply)
{
    // 重新组装数据
    QJsonParseError jsonError;
    // 转化为 JSON 文档
    QJsonDocument doucment = QJsonDocument::fromJson(reply, &jsonError);
    // 解析未发生错误
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {
        // JSON 文档为对象
        if (doucment.isObject()) {
            // 转化为对象
            QJsonObject jsonObj = doucment.object();
            int nType = jsonObj.value("type").toInt();
            QJsonValue dataVal = jsonObj.value("data");

            //通知客户端，该消息已经被服务器收到，客户端可以消除动画
            Q_EMIT sltSendMessage(MsgReceived,dataVal);

            QJsonObject dataObj = dataVal.toObject();
            // 转发的群组id
            int nGroupId = dataObj.value("to").toInt();
            QString strMsg = dataObj.value("msg").toString();
            QString groupHead = DataBase::Instance()->getGroupInfo(nGroupId).value("head").toString();

            // 查询该群组下面的用户，一一转发消息
            QString name = DataBase::Instance()->getUserName(ID);
            qint64 time = dataObj.value("time").toInt();

            // 查询该群组里面的好友
            QJsonArray jsonArr = DataBase::Instance()->getGroupUsers(nGroupId);

            for (int i = 0; i < jsonArr.size(); i++) {
                QJsonObject json = jsonArr.at(i).toObject();
                int nUserId = json.value("id").toInt();

                if (ID != nUserId) {
                    // 重组消息
                    QJsonObject jsonMsg;
                    jsonMsg.insert("group", nGroupId);
                    jsonMsg.insert("groupHead",groupHead);
                    jsonMsg.insert("id", ID);//发送该条群聊消息的人的ID
                    jsonMsg.insert("time",time);
                    jsonMsg.insert("to", nUserId);
                    jsonMsg.insert("msg", strMsg);
                    jsonMsg.insert("tag",1);

                    jsonMsg.insert("name", name);//发送该条群聊消息的人的名字
                    jsonMsg.insert("head", DataBase::Instance()->getUserHead(ID));

                    Q_EMIT signalMsgToClient(quint8(nType), nUserId, jsonMsg);
                }
            }
        }
    }
}

void ClientSocket::parseSendPicture(const QJsonValue &dataVal)
{
    // data 的 value 是对象
    if (dataVal.isObject()) {
        QJsonObject dataObj = dataVal.toObject();
        int id = dataObj.value("from").toInt();

        QJsonObject json;

        QBuffer buffer;
        buffer.open(QIODevice::ReadWrite);
        QPixmap pixmap(MyApp::m_strHeadPath + QString::number(id) + "/" +
                       QString::number(id) + ".png");

        pixmap.save(&buffer, "PNG");
        buffer.close();

        //把登陆消息返回给客户端
        //sltSendMessage(GetPicture,json);
    }
}

void ClientSocket::parseChangePwd(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        //qDebug() << "xjxtest";
        QJsonObject json = dataVal.toObject();
        int id = json.value("id").toInt();
        QString oldPwd = json.value("oldpwd").toString();
        QString newPwd = json.value("newpwd").toString();

        QJsonObject jsonObj = DataBase::Instance()->changePwd(id,oldPwd,newPwd);
        sltSendMessage(ChangePasswd,jsonObj);
    }
}

void ClientSocket::parseFindFriend(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();

        int tag = json.value("tag").toInt();
        int id = json.value("id").toInt();
        QJsonObject jsonObj;
        if(tag == 0){
            jsonObj = DataBase::Instance()->getUserInfo(id);
        }else if(tag == 1){
            jsonObj = DataBase::Instance()->getGroupInfo(id);
        }

        sltSendMessage(FindFriend,jsonObj);
    }
}

void ClientSocket::parseGetOfflineMsg(const QJsonValue &dataVal)
{
    if(dataVal.isObject()){
        QJsonObject json = dataVal.toObject();
        int id = json.value("id").toInt();
        qDebug() << "user:" << id << "request for loading offline msg";

        QJsonArray jsonArr = DataBase::Instance()->getOfflineMsg(id);
        sltSendMessage(GetOfflineMsg,jsonArr);
    }
}

void ClientSocket::sltSendMessage(const quint8 &type, const QJsonValue &jsonVal)
{
    if (!tcpSocket->isOpen())
        return;

    // 构建 Json 对象
    QJsonObject jsonObj;
    jsonObj.insert("type", type);
    jsonObj.insert("from", ID);
    jsonObj.insert("data", jsonVal);

    // 构建 Json 文档
    QJsonDocument document;
    document.setObject(jsonObj);
    tcpSocket->write(document.toJson(QJsonDocument::Indented));
    qDebug() << "-> " << "服务器向用户" << ID << "发送一条消息，"
             << "消息内容为:" << endl << jsonObj << endl;
}


//-----------------------------------------------


ClientFileSocket::ClientFileSocket(QObject *parent, QTcpSocket *socket) : QObject(parent)
{
    // 将整个大的文件分成很多小的部分进行发送，每部分为4字节
    loadSize            = 50 * 1024;
    ullSendTotalBytes   = 0;
    ullRecvTotalBytes   = 0;
    bytesWritten        = 0;
    bytesToWrite        = 0;
    bytesReceived       = 0;

    fileNameSize        = 0;
    busy             = false;

    userID           = -1;
    windowID         = -1;

    // 本地文件存储
    fileToSend = new QFile(this);
    fileToRecv = new QFile(this);

    // 客户端
    if (socket == nullptr)
        tcpSocket = new QTcpSocket(this);
    else
        tcpSocket = socket;

    // 我们更新进度条
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(sltReadyRead()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SIGNAL(signalDisConnected()));

    // 当有数据发送成功时，我们更新进度条
    connect(tcpSocket, SIGNAL(bytesWritten(qint64)),
            this, SLOT(sltUpdateClientProgress(qint64)));
}

ClientFileSocket::~ClientFileSocket()
{

}

void ClientFileSocket::close()
{
    tcpSocket->abort();
}

bool ClientFileSocket::checkUserID(const qint32 nId, const qint32 &winId)
{
    return ((userID == nId) && (windowID == winId));
}

void ClientFileSocket::startTransferFile(QString fileName, int senderID, qint64 time,int flag)
{
    if (busy || !tcpSocket->isOpen())
        return;

    // 要发送的文件
    qDebug() << "服务器准备发送的文件为：" << fileName;
    fileToSend = new QFile(fileName);

    if (!fileToSend->open(QFile::ReadOnly)){
        qDebug() << "open file error!";
        return;
    }

    // 文件总大小
    ullSendTotalBytes = quint64(fileToSend->size());

    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_11);

    QString currentFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);

    // 依次写入[总大小信息空间,文件名大小信息空间,文件发送者ID,文件时间,flag,文件名]
    sendOut << qint64(0) << qint64(0) << qint64(0) << qint64(0) << qint64(0) << currentFileName;

    // 这里的总大小是文件名大小等信息和实际文件大小的总和
    ullSendTotalBytes += quint64(outBlock.size());

    // 返回outBolock的开始，用实际的大小信息代替两个qint64(0)空间
    sendOut.device()->seek(0);
    sendOut << ullSendTotalBytes << qint64((unsigned(outBlock.size()) - sizeof(qint64)*5))
            << qint64(senderID) << time << qint64(flag);

    // 发送完头数据后剩余数据的大小
    bytesToWrite = ullSendTotalBytes - quint64(tcpSocket->write(outBlock));

    outBlock.resize(0);
    busy = true;

    if(senderID == 0){
        qDebug() << "Begin to send file" << fileName
                 << " [ sent by:" << windowID << " recv by:"<< userID << "]";
    }else{
        qDebug() << "Begin to send file" << fileName
                 << " [ sent by:" << senderID << "in group"
                 << windowID << " recv by:"<< userID << "]";
    }
}

void ClientFileSocket::sltUpdateClientProgress(qint64 numBytes)
{
    // 已经发送数据的大小
    bytesWritten += quint64(numBytes);

    // 如果已经发送了数据
    if (bytesToWrite > 0){
        // 每次发送loadSize大小的数据，这里设置为4KB，如果剩余的数据不足4KB，就发送剩余数据的大小
        outBlock = fileToSend->read(qint64(qMin(bytesToWrite, loadSize)));

        // 发送完一次数据后还剩余数据的大小
        bytesToWrite -= quint64(tcpSocket->write(outBlock));

        // 清空发送缓冲区
        outBlock.resize(0);
    } else {
        // 如果没有发送任何数据，则关闭文件
        if (fileToSend->isOpen())
            fileToSend->close();
    }

    // 发送完毕
    if (bytesWritten >= ullSendTotalBytes) {
        if (fileToSend->isOpen())
            fileToSend->close();

        bytesWritten = 0;  // clear fot next send
        ullSendTotalBytes = 0;
        bytesToWrite = 0;
        qDebug() << "send ok" << fileToSend->fileName();
        fileTransFinished();
    }
}

void ClientFileSocket::displayError(QAbstractSocket::SocketError)
{
    tcpSocket->abort();
}

void ClientFileSocket::fileTransFinished()
{
    ullSendTotalBytes   = 0;
    ullRecvTotalBytes   = 0;
    bytesWritten        = 0;
    bytesToWrite        = 0;
    bytesReceived       = 0;

    fileNameSize        = 0;
    busy = false;
}

void ClientFileSocket::sltReadyRead()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_5_11);

    // 连接时的消息
    if (0 == bytesReceived && (-1 == userID) && (-1 == windowID) &&
            (tcpSocket->bytesAvailable() == (sizeof(qint32) * 3))){
        //客户端的TcpFileSocket在连上服务器后会首先发来三个字节的数据，分别是下面三个字段
        in >> userID >> windowID >> tag;
        if(windowID == -2){
            qDebug() << "用户" << userID << "全局收发文件的FileSocket已连接上服务器" << endl;
        }else{
            qDebug() << "聊天双方ID分别为:[" << "sender:" << userID
                     << "  ->  receiver:" << windowID << "]"
                     << "聊天类型为:" << (tag == 0 ? "私聊" : "群聊") << endl;
        }

        Q_EMIT signalConnected();
        return;
    }

    // 如果接收到的数据小于等于24( sizeof(qint64) * 3 )个字节，那么是刚开始接收数据，我们保存为头文件信息
    if (bytesReceived <= (sizeof(qint64)*3)){
        unsigned nlen = sizeof(qint64) * 3;

        // 接收数据总大小信息和文件名大小信息,以及最后的时间戳
        if ((tcpSocket->bytesAvailable() >= nlen) && (fileNameSize == 0)){
            in >> ullRecvTotalBytes >> fileNameSize >> msgSendTime;

            qDebug() << "new file arrived!";
            qDebug() << "msg total size is:" << ullRecvTotalBytes << "bytes"
                     << " filename size is:" << fileNameSize << "bytes"
                     << " send time is:"
                     << QDateTime::fromSecsSinceEpoch(msgSendTime).toString("yyyy-MM-dd  hh:mm:ss");

            if (0 != ullRecvTotalBytes)
                bytesReceived += nlen;
        }

        // 接收文件名，并建立文件
        if((tcpSocket->bytesAvailable() >= qint64(fileNameSize)) &&
                (qint64(fileNameSize) != 0) && (0 != ullRecvTotalBytes)){

            in >> fileReadName;
            bytesReceived += fileNameSize;

            //如果用户是私发文件，则文件保存在服务器该用户个人目录下，
            //如果用户为群发文件，则保存在 GroupDir/UserDir/ 下
            QString strFilePath;
            if(tag == 0){
                strFilePath = MyApp::m_strRecvPath + "User/" + QString::number(userID) + "/";
                MyApp::createDir(strFilePath);
            }else if(tag == 1){
                strFilePath = MyApp::m_strRecvPath + "Group/" + QString::number(windowID) + "/";
                MyApp::createDir(strFilePath);
                strFilePath += QString::number(userID) + "/";
                MyApp::createDir(strFilePath);
            }

            filesavepath = strFilePath;
            fileToRecv->setFileName(strFilePath+fileReadName);
            qDebug() << "file save path is" << strFilePath;


            if (!fileToRecv->open(QFile::WriteOnly | QIODevice::Truncate)){
                qDebug() << "open file error" << fileReadName;
                return;
            }
            qDebug() << "begin to receive file" << fileReadName << "...";
        }
    }

    //如果接收的数据小于总数据，那么写入文件
    if (bytesReceived < ullRecvTotalBytes){
        bytesReceived += quint64(tcpSocket->bytesAvailable());
        inBlock = tcpSocket->readAll();

        if (fileToRecv->isOpen())
            fileToRecv->write(inBlock);

        inBlock.resize(0);
    }

    // 接收数据完成时
    if ((bytesReceived >= ullRecvTotalBytes) && (0 != ullRecvTotalBytes)){
        fileToRecv->close();
        bytesReceived = 0; // clear for next receive
        ullRecvTotalBytes = 0;
        fileNameSize = 0;
        qDebug() << "file received success!" << endl;

        //向数据库插入一条记录
        insertDataBase(filesavepath,fileReadName);

        // 数据接受完成
        fileTransFinished();
    }
}

void ClientFileSocket::insertDataBase(QString filepath,QString filename)
{
    QFileInfo fileinfo(filepath + filename);
    qint64 filesize = 0;
    if(fileinfo.exists()){
        filesize = fileinfo.size();
    }

    QString sendtime = QDateTime::fromSecsSinceEpoch(msgSendTime).toString("yyyy-MM-dd  hh:mm:ss");
    qDebug() << "file sent time:" << msgSendTime << sendtime;
    QSqlQuery query;

    query.prepare("INSERT INTO FileSent (senderID, receiverID, groupID, "
                  "filename, filesize, filepath, time, tag, type) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);");
    query.bindValue(0, userID);
    query.bindValue(3, filename);
    query.bindValue(4, filesize);
    query.bindValue(5, filepath);
    query.bindValue(6, sendtime);
    query.bindValue(7, 0);
    query.bindValue(8, tag);

    if(tag == 0){//私发文件
        query.bindValue(1, windowID);
        query.bindValue(2, 0);
        query.exec();
        //qDebug() << query.lastError().text();

        if(fileinfo.suffix().toLower() == "png" || fileinfo.suffix().toLower() == "jpg"){
            //通知发送该图片的客户端已经收到该图片,客户端消除动画
            {
                QJsonObject json;
                json.insert("from",userID);
                json.insert("to",windowID);
                json.insert("time",msgSendTime);
                emit signalMsgToClient(MsgReceived,userID,json);
            }

            //此处需要先判断用户是否在线，若不在线，则不发送，等到下次用户启动客户端再发送
            int status = DataBase::Instance()->getUserLineStatus(windowID);
            if(status == OnLine){//用户在线
                //通知接收该图片的客户端打开tcp连接来接受该图片，因为若对方的这个聊天窗口没有打开是没法接收该文件的
                {   QJsonObject json;
                    json.insert("id",userID);
                    json.insert("to",windowID);
                    json.insert("time",msgSendTime);
                    json.insert("tag",0);
                    json.insert("type",Picture);
                    QJsonObject jsonObj = DataBase::Instance()->getUserInfo(windowID);
                    QString name = jsonObj.value("name").toString();
                    json.insert("name",name);
                    emit signalMsgToClient(SendPicture,windowID,json);
                }

                myHelper::Sleep(500);//等待500毫秒

                //如果客户端在线，500毫秒后可以确定客户端打开了连接，可以进行文件传输
                //若用户不在线，则需要把未传输的文件记录到数据库中，等到用户上线后重新传送

                //这个时候可以直接把图片文件发往目标用户
                {
                    QJsonObject json2;
                    json2.insert("tag",0);//群聊私聊标记
                    json2.insert("from",userID);//文件来自谁
                    json2.insert("to",windowID);//文件发往谁
                    json2.insert("group",0);//群id，没有就设为0
                    json2.insert("filename",filename);
                    json2.insert("time",msgSendTime);
                    emit signalDownloadFile(json2);
                }
            }else{//用户不在线，记录离线消息到数据库
                QJsonObject json;
                json.insert("id",userID);
                json.insert("to",windowID);
                json.insert("time",msgSendTime);
                json.insert("tag",0);
                json.insert("type",Picture);
                QJsonObject jsonObj = DataBase::Instance()->getUserInfo(windowID);
                QString name = jsonObj.value("name").toString();
                json.insert("name",name);
                json.insert("msg",filename);
                json.insert("fileSize",filesize);
                emit signalMsgToClient(SendPicture,windowID,json);
            }
        }else{//普通文件，通知用户下载
            QJsonObject json;
            json.insert("id",userID);
            json.insert("time",msgSendTime);
            json.insert("fileName",filename);
            json.insert("fileSize",filesize);
            json.insert("tag",tag);//私发文件还是群发文件
            json.insert("msg","");
            json.insert("group",0);
            json.insert("type",Files);

            QJsonObject jsonObj = DataBase::Instance()->getUserInfo(windowID);
            QString name = jsonObj.value("name").toString();
            json.insert("name",name);
            json.insert("msg",filename);
            json.insert("fileSize",filesize);

            //通知用户下载文件
            Q_EMIT signalMsgToClient(SendFile, windowID, json);//(type,reveicerID,data)
        }
    }else if(tag == 1){//群发文件
        query.bindValue(2, windowID);

        QJsonArray array = DataBase::Instance()->getGroupUsers(windowID);
        for (int i = 0; i < array.size(); i++) {
            QJsonObject json = array.at(i).toObject();
            int id = json.value("id").toInt();//要发送的目标用户id

            if(id == userID){//通知发送该图片的客户端已经收到该文件,客户端消除动画
                if(fileinfo.suffix().toLower() == "png" || fileinfo.suffix().toLower() == "jpg"){
                    QJsonObject json;
                    json.insert("from",userID);
                    json.insert("to",windowID);
                    json.insert("time",msgSendTime);

                    emit signalMsgToClient(MsgReceived,userID,json);
                }
            }else{
                //不要给自己发
                query.bindValue(1, id);
                query.exec();
                //qDebug() << query.lastError().text();

                if(fileinfo.suffix().toLower() == "png" || fileinfo.suffix().toLower() == "jpg"){
                    //此处需要先判断用户是否在线，若不在线，则不发送，等到下次用户启动客户端再发送
                    int status = DataBase::Instance()->getUserLineStatus(id);
                    if(status == OnLine){
                        //通知接收该图片的客户端打开tcp连接来接受该图片，因为若对方的这个聊天窗口没有打开是没法接收该文件的
                        {
                            QJsonObject json;
                            json.insert("id",userID);
                            json.insert("group",windowID);
                            json.insert("time",msgSendTime);
                            json.insert("tag",1);
                            json.insert("type",Picture);
                            QJsonObject jsonObj = DataBase::Instance()->getUserInfo(id);
                            QString name = jsonObj.value("name").toString();
                            json.insert("name",name);
                            emit signalMsgToClient(SendPicture,id,json);
                        }

                        myHelper::Sleep(500);//等待500毫秒

                        //如果客户端在线，500毫秒后可以确定客户端打开了连接，可以进行文件传输
                        //若用户不在线，则需要把未传输的文件记录到数据库中，等到用户上线后重新传送

                        //这个时候可以直接把图片文件发往目标用户
                        {
                            QJsonObject json2;
                            json2.insert("tag",1);//群聊私聊标记
                            json2.insert("from",userID);//文件来自谁
                            json2.insert("to",id);//文件发往谁
                            json2.insert("group",windowID);//群id，没有就设为0
                            json2.insert("filename",filename);
                            json2.insert("time",msgSendTime);
                            emit signalDownloadFile(json2);
                        }
                    }else{//用户离线
                        QJsonObject json;
                        json.insert("id",userID);
                        json.insert("group",windowID);
                        json.insert("time",msgSendTime);
                        json.insert("tag",1);
                        json.insert("type",Picture);
                        QJsonObject jsonObj = DataBase::Instance()->getUserInfo(id);
                        QString name = jsonObj.value("name").toString();
                        json.insert("name",name);
                        json.insert("msg",filename);
                        json.insert("fileSize",filesize);
                        emit signalMsgToClient(SendPicture,id,json);
                    }
                }else{//普通文件，通知用户下载
                    QJsonObject json;
                    json.insert("id",userID);
                    json.insert("time",msgSendTime);
                    json.insert("fileName",filename);
                    json.insert("fileSize",filesize);
                    json.insert("tag",tag);//私发文件还是群发文件
                    json.insert("msg","");
                    json.insert("group",windowID);
                    json.insert("type",Files);

                    QJsonObject jsonObj = DataBase::Instance()->getUserInfo(id);
                    QString name = jsonObj.value("name").toString();
                    json.insert("name",name);
                    json.insert("msg",filename);
                    json.insert("fileSize",filesize);

                    //通知用户下载文件
                    Q_EMIT signalMsgToClient(SendFile, id, json);//(type,reveicerID,data)
                }
            }
        }
    }
}


