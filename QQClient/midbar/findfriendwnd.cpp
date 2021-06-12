#include "findfriendwnd.h"
#include "myapp.h"
#include "unit.h"
#include "global.h"
#include "database.h"

#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QJsonObject>
#include <QDebug>
#include <QFileInfo>

FindFriendWnd::FindFriendWnd(int tag)
    : QDialog(),tag(tag)
{
    setFixedSize(700,250);
    setWindowFlags(Qt::WindowCloseButtonHint);

    QLabel *label = new QLabel(this);
    if(tag == 0)
        label->setText("找好友");
    else if(tag == 1)
        label->setText("找群");
    QFont font = QFont("Microsoft YaHei", 18, 50, false);
    QFont labelFont = QFont("Microsoft YaHei", 12, 50, false);
    QFont resultFont = QFont("Microsoft YaHei", 14, 50, false);
    label->setFont(font);

    label->setGeometry(300,30,100,30);

    searchBar = new SearchBar(this,QSize(500,50),1);
    searchBar->setGeometry(40,80,500,50);

    QPushButton *searchBtn = new QPushButton("搜索",this);
    searchBtn->setGeometry(560,85,100,40);
    searchBtn->setStyleSheet("QPushButton{border:1px solid #86949e;background-color:#0188fb;border-radius:5px}"
                             "QPushButton:hover{background-color:#289cff;}"
                             "QPushButton:pressed{background-color:#0081ef}");
    connect(searchBtn,SIGNAL(clicked(bool)),this,SLOT(sltBtnClicked()));

    QPalette palette;
    palette.setColor(QPalette::Background, Qt::white);
    this->setAutoFillBackground(true);  //一定要这句，否则不行
    this->setPalette(palette);

    resultLabel = new QLabel(this);
    resultLabel->setVisible(false);
    resultLabel->setGeometry(270,160,250,30);
    resultLabel->setFont(resultFont);

    idLabel = new QLabel(this);
    idLabel->setVisible(false);
    idLabel->setFont(labelFont);
    idLabel->setFixedSize(80,30);
    idLabel->move(370,250);

    nameLabel = new QLabel(this);
    nameLabel->setVisible(false);
    nameLabel->setFont(labelFont);
    nameLabel->setFixedSize(80,30);
    nameLabel->move(370,280);

    headLabel = new RoundLabel(this);
    headLabel->setVisible(false);
    headLabel->setFont(labelFont);
    headLabel->setFixedSize(100,100);
    headLabel->move(250,230);

    addBtn = new QPushButton(this);
    addBtn->setGeometry(300,350,80,30);
    addBtn->setStyleSheet("QPushButton{border:1px solid #86949e;background-color:#0188fb;border-radius:5px}"
                          "QPushButton:hover{background-color:#289cff;}"
                          "QPushButton:pressed{background-color:#0081ef}");
    addBtn->setVisible(false);
    connect(addBtn,SIGNAL(clicked(bool)),this,SLOT(sltAddFriend()));

    msgLabel = new QLabel("已发送请求！",this);
    msgLabel->setFont(resultFont);
    msgLabel->setGeometry(280,370,150,30);
    msgLabel->setStyleSheet("color:#0081ef");
    msgLabel->setVisible(false);

    this->setFocusPolicy(Qt::StrongFocus);
    this->setStyleSheet("QDialog{border-radius:5px}");
}

void FindFriendWnd::sltBtnClicked()
{
    this->setFixedSize(700,500);
    myHelper::FormInCenter(this);

    QString text = searchBar->text();
    int cnt = text.size();
    for(int i = 0;i < cnt;i++){
        if(text[i] >= 48 && text[i] <= 57){
            continue;
        }else{
            QMessageBox::information(this,"错误","用户id包含非法字符");
            return;
        }
    }

    resultLabel->setVisible(false);
    idLabel->setVisible(false);
    nameLabel->setVisible(false);
    headLabel->setVisible(false);
    addBtn->setVisible(false);
    msgLabel->setVisible(false);

    int id = text.toInt();
    QJsonObject json;
    json.insert("tag",tag);
    json.insert("id",id);

    emit signalFind(json);
}

void FindFriendWnd::sltFindFriendReply(const QJsonValue &jsonVal)
{
    if(jsonVal.isObject()){
        QJsonObject json = jsonVal.toObject();
        //qDebug() << "fineWnd:" << json;


        int code = json.value("code").toInt();
        if(code == -1){
            if(tag == 0)
                resultLabel->setText("没有查询到该用户");
            else if(tag == 1)
                resultLabel->setText("没有查询到该群");
            resultLabel->show();
        }else{
            int id = json.value("id").toInt();
            QString name = json.value("name").toString();
            QString head = json.value("head").toString();

            QFileInfo fileInfo(MyApp::m_strHeadPath + head);

            if(!fileInfo.exists()){
                QJsonObject json;
                json.insert("tag",-2);
                json.insert("from",MyApp::m_nId);
                json.insert("id",-2);
                json.insert("who",id);

                emit signalSendMessage(GetPicture, json);
                myHelper::Sleep(500);//等待半秒

                QString headPath = MyApp::m_strHeadPath + QString::number(id) + ".png";
                QFileInfo fileInfo_(headPath);

                if(!fileInfo_.exists()){
                    head = ":/Icons/MainWindow/default_head_icon.png";//没有收到则显示默认头像
                }else{
                    head = headPath;
                }
            }else{
                head = MyApp::m_strHeadPath + head;
            }

            resultLabel->setText("查询到下列信息");
            resultLabel->setVisible(true);
            idLabel->setText(QString::number(id));
            idLabel->setVisible(true);
            nameLabel->setText(name);
            nameLabel->setVisible(true);
            qDebug() << "head path:" << head;
            headLabel->setPixmap(head);
            headLabel->setVisible(true);
            if(tag == 0)
                addBtn->setText("添加好友");
            else if(tag == 1)
                addBtn->setText("加入该群");
            addBtn->setVisible(true);

            friendID = id;
        }
    }
}

void FindFriendWnd::sltAddFriend()
{
    QJsonObject json;
    json.insert("id",friendID);
    json.insert("sender",MyApp::m_nId);

    if(tag == 0){
        if(friendID == MyApp::m_nId){
            QMessageBox::information(this,"错误","无法添加自己为好友");
        }else{
            bool flag = DataBase::Instance()->isMyFriend(friendID);
            if(flag){
                QMessageBox::information(this,"错误","您和该用户已经是好友了");
            }else{
                msgLabel->setVisible(true);
                addBtn->setVisible(false);
                emit signalSendMessage(AddFriend, json);
            }
        }
    }else if(tag == 1){
        bool flag = DataBase::Instance()->isInGroup(friendID);
        if(flag){
            QMessageBox::information(this,"错误","您已经加入该群了");
        }else{
            msgLabel->setVisible(true);
            addBtn->setVisible(false);
            emit signalSendMessage(AddGroup, json);
        }
    }
}
