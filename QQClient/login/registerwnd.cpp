#include "RegisterWnd.h"
#include "unit.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>

RegisterWndPrivate::RegisterWndPrivate(QWidget *parent)
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
    connect(m_menuCloseBtn, &QPushButton::pressed, this, &RegisterWndPrivate::closeWnd);

    m_menuMinBtn = new QPushButton(this);
    m_menuMinBtn->setFlat(true);
    m_menuMinBtn->setFixedSize(30, 32);
    m_menuMinBtn->move(370, 0);
    m_menuMinBtn->setStyleSheet("QPushButton{ border-image: url(:/loginwnd/min_normal); }"
                                "QPushButton:hover:!pressed{ border-image: url(:/loginwnd/min_hover); }"
                                "QPushButton:hover:pressed{ border-image: url(:/loginwnd/min_press); border-style:none; }");
    connect(m_menuMinBtn, &QPushButton::pressed, this, &RegisterWndPrivate::showMinimized);

    welcome = new QLabel("欢迎注册",this);
    welcome->setFont(font);
    logo = new QLabel(this);
    logo->setPixmap(QPixmap(":/Icons/MainWindow/title.png").
                    scaled(80,80,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    nameLabel = new QLabel("昵称",this);
    nameLabel->setFont(font2);
    passwordLabel = new QLabel("密码",this);
    passwordLabel->setFont(font2);
    nameEdit = new QLineEdit(this);
    connect(nameEdit,&QLineEdit::textChanged,[&](){
        m_notifyMsg->hide();
        this->setFixedSize(430,330);
    });

    passwordEdit = new QLineEdit(this);
    connect(passwordEdit,&QLineEdit::textChanged,[&](){
        m_notifyMsg->hide();
        this->setFixedSize(430,330);
    });

    okBtn = new QPushButton("注册",this);
    okBtn->setStyleSheet("QPushButton{border:1px solid #86949e;background-color:#f4f4f4;border-radius:3px}"
                         "QPushButton:hover{background-color:#bee7fd;}"
                         "QPushButton:pressed{background-color:#f4f4f4}");
    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(sltBtnClicked()));

    cancelBtn = new QPushButton("取消",this);
    cancelBtn->setStyleSheet("QPushButton{border:1px solid #86949e;background-color:#f4f4f4;border-radius:3px}"
                             "QPushButton:hover{background-color:#bee7fd;}"
                             "QPushButton:pressed{background-color:#f4f4f4}");
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(closeWnd()));
    idLabel = new QLabel(this);
    idLabel->setFont(font2);
    idLabel->setGeometry(80,250,60,30);
    idLabel->setVisible(false);

    welcome->setGeometry(80,60,140,30);
    logo->setGeometry(270,35,80,80);

    nameLabel->setGeometry(90,140,60,30);
    nameEdit->setGeometry(160,143,180,25);
    nameEdit->setStyleSheet("background:transparent;border-width:1px;border-style:outset");

    passwordLabel->setGeometry(90,190,60,30);
    passwordEdit->setGeometry(160,193,180,25);
    passwordEdit->setStyleSheet("background:transparent;border-width:1px;border-style:outset");

    okBtn->setGeometry(180,270,100,30);
    cancelBtn->setGeometry(300,270,100,30);
}

void RegisterWndPrivate::sltBtnClicked()
{
    qDebug() << "开始注册";
    if(nameEdit->text().isEmpty()){
        this->setFixedSize(430,350);
        m_notifyMsg->move(0,330);
        m_notifyMsg->setText("  请输入昵称");
        m_notifyMsg->show();
        return;
    }else{
        if(nameEdit->text().size() > 20){
            this->setFixedSize(430,350);
            m_notifyMsg->move(0,330);
            m_notifyMsg->setText("  昵称过长!请输入少于20个字符的昵称");
            m_notifyMsg->show();
            return;
        }else{
            if(passwordEdit->text().isEmpty()){
                this->setFixedSize(430,350);
                m_notifyMsg->move(0,330);
                m_notifyMsg->setText("  请输入密码");
                m_notifyMsg->show();
                return;
            }else{
                if(passwordEdit->text().size() > 20){
                    this->setFixedSize(430,350);
                    m_notifyMsg->move(0,330);
                    m_notifyMsg->setText("  密码过长!请输入少于20个字符的密码");
                    m_notifyMsg->show();
                    return;
                }else{
                    QString text = passwordEdit->text();
                    int len = text.size();
                    for(int i = 0;i < len;i++){
                        if( (text[i] >= 65 && text[i] <= 90) ||
                                (text[i] >= 97 && text[i] <= 122) ||
                                (text[i] >= 48 && text[i] <= 57) ||
                                text[i] == '_'){//合法字符
                            continue;
                        }else{
                            this->setFixedSize(430,350);
                            m_notifyMsg->move(0,330);
                            m_notifyMsg->setText("  密码中包含非法字符!只能包含英文字母，数字，下划线");
                            m_notifyMsg->show();
                            return;
                        }
                    }

                    m_notifyMsg->hide();
                    this->setFixedSize(430,330);

                    //向服务器发送注册消息
                    QJsonObject json;
                    json.insert("name",nameEdit->text());
                    json.insert("pwd",passwordEdit->text());
                    emit signalRegister(json);

                    nameEdit->clear();
                    passwordEdit->clear();
                }
            }
        }
    }
}

void RegisterWndPrivate::closeWnd()
{
    nameEdit->clear();
    passwordEdit->clear();

    emit closeWindow();
}

void RegisterWndPrivate::sltRegisterOK(const QJsonValue &jsonVal)
{
    if(jsonVal.isObject()){
        QJsonObject json = jsonVal.toObject();
        int id = json.value("id").toInt();
        QString pwd = json.value("pwd").toString();
        QString name = json.value("name").toString();

        qDebug() << "注册成功!"
                 << " id:" << id
                 << " name:" << name
                 << " pwd:" << pwd << endl;

        welcome->setText("注册成功!");
        nameEdit->setVisible(false);
        passwordEdit->setVisible(false);

        nameLabel->setGeometry(80,120,250,30);
        nameLabel->setText(name + ",欢迎使用.");

        idLabel->setGeometry(80,170,350,30);
        idLabel->setVisible(true);
        idLabel->setText("您的id为: " + QString::number(id));

        passwordLabel->setGeometry(80,210,250,30);
        passwordLabel->setText("密码为: " + pwd);

        okBtn->setText("返回登陆");
        disconnect(okBtn,SIGNAL(clicked(bool)),this,SLOT(sltBtnClicked()));
        connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(closeWnd()));
    }
}


//---------------------------------


RegisterWnd::RegisterWnd(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    mainWnd = new RegisterWndPrivate;

    QHBoxLayout *pLayout = new QHBoxLayout(this);
    pLayout->addWidget(mainWnd);
    pLayout->setContentsMargins(20, 20, 20, 20);

    QGraphicsDropShadowEffect *pEffect = new QGraphicsDropShadowEffect(mainWnd);
    pEffect->setOffset(0, 0);
    pEffect->setColor(QColor(QStringLiteral("black")));
    pEffect->setBlurRadius(30);
    mainWnd->setGraphicsEffect(pEffect);

    connect(mainWnd,SIGNAL(closeWindow()),this,SLOT(sltCloseWnd()));
    connect(mainWnd,SIGNAL(signalRegister(const QJsonValue &)),
            this,SIGNAL(signalRegister(const QJsonValue &)));
}

void RegisterWnd::sltCloseWnd()
{
    emit closeWindow(this->pos());
    this->close();
}

void RegisterWnd::sltRegisterOK(const QJsonValue &dataVal)
{
    mainWnd->sltRegisterOK(dataVal);
}

void RegisterWnd::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_pressedPoint = event->globalPos() - this->pos();
        m_isPressed = true;
        event->accept();
    }
}

void RegisterWnd::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressed = false;
    event->accept();
}

void RegisterWnd::mouseMoveEvent(QMouseEvent *event)
{
    if((event->buttons()&(Qt::LeftButton)) && m_isPressed == true){
        move(event->globalPos() - m_pressedPoint);
        event->accept();
    }
}
