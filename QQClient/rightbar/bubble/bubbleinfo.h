#ifndef BUBBLEINFO_H
#define BUBBLEINFO_H

#include "unit.h"

#include <QDateTime>
#include <QObject>

typedef enum Sender{
    Me = 0,
    You = 1,
    System = 2
}Sender;

class BubbleInfo : public QObject
{
    Q_OBJECT
public:
    BubbleInfo();

    Sender sender;//消息发送者
    int myID = 0;
    int yourID = 0;
    int groupID = 0;
    int tag = 0;//0表示私聊消息，1表示群聊消息

    QString msg = "";//消息内容
    QString name;//发送者的名字
    QString headIcon;//头像

    //消息发送的时间戳 qint64 curTime = QDateTime::currentSecsSinceEpoch();
    qint64 time = 0;

    MessageType msgType;//消息的类型，普通文本消息，图片，文件
    qint64 fileSize = 0;//文件大小,只有当消息类型为文件时有效

    bool showProgressBar = true;//是否显示进度条
    bool showAnimation = true;
    bool showDownload = false;
    bool showError = false;//删除好友或者群时再发送消息时，消息无法发送，此时显示一个感叹号

    bool downloaded = false;//是否已经下载了

signals:
    void updateProgressBar(qint64,qint64);
    void updatePopMenu();
    void hideAnimation();
};

#endif // BUBBLEINFO_H
