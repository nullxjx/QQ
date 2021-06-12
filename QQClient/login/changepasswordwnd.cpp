#include "changepasswordwnd.h"
#include "unit.h"
#include "windwmapi.h"

#include <QDebug>
#include <QPainter>
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>

#pragma comment(lib, "user32.lib")

ChangePasswordWndPrivate::ChangePasswordWndPrivate(QWidget *parent)
    : WindowBase(parent)
{
    QFont font = QFont("Microsoft YaHei", 18, 50, false);
    QFont font2 = QFont("Microsoft YaHei", 12, 48, false);

    this->setFixedSize(430,330);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    m_notifyMsg= new QLabel(this);
    m_notifyMsg->setFixedSize(430, 20);
    m_notifyMsg->setStyleSheet("background-color:#09a3dc;font-size: 12px;font-family:Microsoft YaHei;");
    m_notifyMsg->hide();

    m_menuCloseBtn = new QPushButton(this);
    m_menuCloseBtn->setFlat(true);
    m_menuCloseBtn->setFixedSize(30, 32);
    m_menuCloseBtn->move(400, 0);
    m_menuCloseBtn->setStyleSheet("QPushButton{ border-image: url(:/loginwnd/close_normal); }"
                                  "QPushButton:hover:!pressed{ border-image: url(:/loginwnd/close_hover); }"
                                  "QPushButton:hover:pressed{ border-image: url(:/loginwnd/close_press); border-style:none; }");
    connect(m_menuCloseBtn, &QPushButton::pressed, this, &ChangePasswordWndPrivate::closeWnd);

    m_menuMinBtn = new QPushButton(this);
    m_menuMinBtn->setFlat(true);
    m_menuMinBtn->setFixedSize(30, 32);
    m_menuMinBtn->move(370, 0);
    m_menuMinBtn->setStyleSheet("QPushButton{ border-image: url(:/loginwnd/min_normal); }"
                                "QPushButton:hover:!pressed{ border-image: url(:/loginwnd/min_hover); }"
                                "QPushButton:hover:pressed{ border-image: url(:/loginwnd/min_press); border-style:none; }");
    connect(m_menuMinBtn, &QPushButton::pressed, this, &ChangePasswordWndPrivate::showMinimized);

    title = new QLabel("修改密码",this);
    title->setFont(font);
    logo = new QLabel(this);
    logo->setPixmap(QPixmap(":/Icons/MainWindow/title.png").
                    scaled(80,80,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    idLabel = new QLabel("用户id",this);
    idLabel->setFont(font2);
    oldPasswordLabel = new QLabel("旧密码",this);
    oldPasswordLabel->setFont(font2);
    newPasswordLabel = new QLabel("新密码",this);
    newPasswordLabel->setFont(font2);


    idEdit = new QLineEdit(this);
    connect(idEdit,&QLineEdit::textChanged,[&](){
        m_notifyMsg->hide();
        this->setFixedSize(430,330);
    });

    oldPasswordEdit = new QLineEdit(this);
    connect(oldPasswordEdit,&QLineEdit::textChanged,[&](){
        m_notifyMsg->hide();
        this->setFixedSize(430,330);
    });

    newPasswordEdit = new QLineEdit(this);
    connect(oldPasswordEdit,&QLineEdit::textChanged,[&](){
        m_notifyMsg->hide();
        this->setFixedSize(430,330);
    });

    okBtn = new QPushButton("修改密码",this);
    okBtn->setStyleSheet("QPushButton{border:1px solid #86949e;background-color:#f4f4f4;border-radius:3px}"
                         "QPushButton:hover{background-color:#bee7fd;}"
                         "QPushButton:pressed{background-color:#f4f4f4}");
    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(sltOKBtnClicked()));


    cancelBtn = new QPushButton("取消",this);
    cancelBtn->setStyleSheet("QPushButton{border:1px solid #86949e;background-color:#f4f4f4;border-radius:3px}"
                             "QPushButton:hover{background-color:#bee7fd;}"
                             "QPushButton:pressed{background-color:#f4f4f4}");
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(closeWnd()));

    title->setGeometry(80,60,140,30);
    logo->setGeometry(270,35,80,80);

    idLabel->setGeometry(90,130,60,30);
    idEdit->setGeometry(160,133,180,25);
    idEdit->setStyleSheet("background:transparent;border-width:1px;border-style:outset");

    oldPasswordLabel->setGeometry(90,170,60,30);
    oldPasswordEdit->setGeometry(160,173,180,25);
    oldPasswordEdit->setStyleSheet("background:transparent;border-width:1px;border-style:outset");

    newPasswordLabel->setGeometry(90,210,60,30);
    newPasswordEdit->setGeometry(160,213,180,25);
    newPasswordEdit->setStyleSheet("background:transparent;border-width:1px;border-style:outset");

    okBtn->setGeometry(180,270,100,30);
    cancelBtn->setGeometry(300,270,100,30);

}

void ChangePasswordWndPrivate::closeWnd()
{
    idEdit->clear();
    oldPasswordEdit->clear();
    newPasswordEdit->clear();

    emit closeWindow();
}

void ChangePasswordWndPrivate::sltOKBtnClicked()
{
    QString id = idEdit->text();
    QString oldPwd = oldPasswordEdit->text();
    QString newPwd = newPasswordEdit->text();

    if(id.isEmpty()){
        this->setFixedSize(430,350);
        m_notifyMsg->move(0,330);
        m_notifyMsg->setText("  请输入要找回密码的账户id!");
        m_notifyMsg->show();
        return;
    }else{
        int cnt = id.size();
        for(int i = 0;i < cnt;i++){
            if(id[i] >= 48 && id[i] <= 57){
                continue;
            }else{
                this->setFixedSize(430,350);
                m_notifyMsg->move(0,330);
                m_notifyMsg->setText("  id只能包含数字!");
                m_notifyMsg->show();
                return;
            }
        }

        if(oldPwd.isEmpty()){
            this->setFixedSize(430,350);
            m_notifyMsg->move(0,330);
            m_notifyMsg->setText("请输入旧密码!");
            m_notifyMsg->show();
            return;
        }else{
            int cnt = oldPwd.size();
            if(cnt > 20){
                this->setFixedSize(430,350);
                m_notifyMsg->move(0,330);
                m_notifyMsg->setText("  旧密码长度过长，应少于20个字符!");
                m_notifyMsg->show();
                return;
            }else{
                for(int i = 0;i < cnt;i++){
                    if( (oldPwd[i] >= 65 && oldPwd[i] <= 90) ||
                            (oldPwd[i] >= 97 && oldPwd[i] <= 122) ||
                            (oldPwd[i] >= 48 && oldPwd[i] <= 57) ||
                            oldPwd[i] == '_'){//合法字符
                        continue;
                    }else{
                        this->setFixedSize(430,350);
                        m_notifyMsg->move(0,330);
                        m_notifyMsg->setText("  旧密码中包含非法字符!只能包含英文字母，数字，下划线");
                        m_notifyMsg->show();
                        return;
                    }
                }

                if(newPwd.isEmpty()){
                    this->setFixedSize(430,350);
                    m_notifyMsg->move(0,330);
                    m_notifyMsg->setText("  请输入新密码!");
                    m_notifyMsg->show();
                    return;
                }else{
                    int cnt = newPwd.size();
                    if(cnt > 20){
                        this->setFixedSize(430,350);
                        m_notifyMsg->move(0,330);
                        m_notifyMsg->setText("  新密码长度过长，应少于20个字符!");
                        m_notifyMsg->show();
                        return;
                    }else{
                        for(int i = 0;i < cnt;i++){
                            if( (newPwd[i] >= 65 && newPwd[i] <= 90) ||
                                    (newPwd[i] >= 97 && newPwd[i] <= 122) ||
                                    (newPwd[i] >= 48 && newPwd[i] <= 57) ||
                                     newPwd[i] == '_'){//合法字符
                                continue;
                            }else{
                                this->setFixedSize(430,350);
                                m_notifyMsg->move(0,330);
                                m_notifyMsg->setText("  新密码中包含非法字符!只能包含英文字母，数字，下划线");
                                m_notifyMsg->show();
                                return;
                            }
                        }

                        m_notifyMsg->hide();
                        this->setFixedSize(430,330);

                        //向服务器发送修改密码消息

                        //向服务器发送注册消息
                        QJsonObject json;
                        json.insert("id",id.toInt());
                        json.insert("oldpwd",oldPwd);
                        json.insert("newpwd",newPwd);

                        qDebug() << "开始找回密码";
                        emit signalChangePwd(json);

                        idEdit->clear();
                        oldPasswordEdit->clear();
                        newPasswordEdit->clear();
                    }
                }
            }
        }
    }
}

void ChangePasswordWndPrivate::sltChangePwdReply(const QJsonValue &jsonVal)
{
    if(jsonVal.isObject()){
        QJsonObject json = jsonVal.toObject();
        int code = json.value("code").toInt();
        QString newpwd = json.value("newpwd").toString();
        if(code == 0){
            qDebug() << "修改密码成功!";
            idEdit->setVisible(false);
            oldPasswordEdit->setVisible(false);
            oldPasswordLabel->setVisible(false);
            newPasswordEdit->setVisible(false);

            idLabel->setGeometry(90,140,200,30);
            idLabel->setText("修改密码成功!");

            newPasswordLabel->setGeometry(90,180,250,30);
            newPasswordLabel->setText("您的新密码为: " + newpwd);

            disconnect(okBtn,SIGNAL(clicked(bool)),this,SLOT(sltOKBtnClicked()));
            okBtn->setText("返回登陆");
            connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(closeWnd()));
        }else if(code == -1){
            //原密码不正确
            this->setFixedSize(430,350);
            m_notifyMsg->move(0,330);
            m_notifyMsg->setText("该账户原密码错误!");
            m_notifyMsg->show();
        }else if(code == -2){
            //用户名不存在
            this->setFixedSize(430,350);
            m_notifyMsg->move(0,330);
            m_notifyMsg->setText("该账户不存在!");
            m_notifyMsg->show();
        }
    }
}


//--------------------------------


ChangePasswordWnd::ChangePasswordWnd(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    mainWnd = new ChangePasswordWndPrivate;

    QHBoxLayout *pLayout = new QHBoxLayout(this);
    pLayout->addWidget(mainWnd);
    pLayout->setContentsMargins(20, 20, 20, 20);

    QGraphicsDropShadowEffect *pEffect = new QGraphicsDropShadowEffect(mainWnd);
    pEffect->setOffset(0, 0);
    pEffect->setColor(QColor(QStringLiteral("black")));
    pEffect->setBlurRadius(30);
    mainWnd->setGraphicsEffect(pEffect);

    connect(mainWnd,SIGNAL(closeWindow()),this,SLOT(sltCloseWnd()));
    connect(mainWnd,SIGNAL(signalChangePwd(const QJsonValue &)),
            this,SIGNAL(signalChangePwd(const QJsonValue &)));
}

void ChangePasswordWnd::sltChangePwdReply(const QJsonValue &dataVal)
{
    mainWnd->sltChangePwdReply(dataVal);
}

void ChangePasswordWnd::sltCloseWnd()
{
    emit closeWindow(this->pos());
    this->close();
}

void ChangePasswordWnd::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_pressedPoint = event->globalPos() - this->pos();
        m_isPressed = true;
        event->accept();
    }
}

void ChangePasswordWnd::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressed = false;
    event->accept();
}

void ChangePasswordWnd::mouseMoveEvent(QMouseEvent *event)
{
    if((event->buttons()&(Qt::LeftButton)) && m_isPressed == true){
        move(event->globalPos() - m_pressedPoint);
        event->accept();
    }
}
