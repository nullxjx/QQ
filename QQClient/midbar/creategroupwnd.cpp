#include "creategroupwnd.h"

#include "myapp.h"

#include <QJsonObject>
#include <QDateTime>
#include <QDebug>

CreateGroupWnd::CreateGroupWnd() : QDialog ()
{
    QFont font = QFont("Microsoft YaHei", 18, 50, false);
    QFont font2 = QFont("Microsoft YaHei", 12, 48, false);

    this->setFixedSize(600,400);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    m_notifyMsg= new QLabel(this);
    m_notifyMsg->setFixedSize(600, 20);
    m_notifyMsg->setStyleSheet("background-color:#09a3dc;font-size: 12px;font-family:Microsoft YaHei;");
    m_notifyMsg->hide();

    m_menuCloseBtn = new QPushButton(this);
    m_menuCloseBtn->setFlat(true);
    m_menuCloseBtn->setFixedSize(30, 32);
    m_menuCloseBtn->move(570, 0);
    m_menuCloseBtn->setStyleSheet("QPushButton{ border-image: url(:/loginwnd/close_normal); }"
                                  "QPushButton:hover:!pressed{ border-image: url(:/loginwnd/close_hover); }"
                                  "QPushButton:hover:pressed{ border-image: url(:/loginwnd/close_press); border-style:none; }");
    connect(m_menuCloseBtn, &QPushButton::pressed, this, &CreateGroupWnd::close);

    m_menuMinBtn = new QPushButton(this);
    m_menuMinBtn->setFlat(true);
    m_menuMinBtn->setFixedSize(30, 32);
    m_menuMinBtn->move(540, 0);
    m_menuMinBtn->setStyleSheet("QPushButton{ border-image: url(:/loginwnd/min_normal); }"
                                "QPushButton:hover:!pressed{ border-image: url(:/loginwnd/min_hover); }"
                                "QPushButton:hover:pressed{ border-image: url(:/loginwnd/min_press); border-style:none; }");
    connect(m_menuMinBtn, &QPushButton::pressed, this, &CreateGroupWnd::showMinimized);

    welcome = new QLabel("创建群聊",this);
    welcome->setStyleSheet("color:white");
    welcome->setFont(font);

    logo = new QLabel(this);
    logo->setPixmap(QPixmap(":/Icons/MainWindow/title.png").
                    scaled(80,80,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    nameLabel = new QLabel("输入群名称",this);
    nameLabel->setFont(font2);
    nameLabel->setStyleSheet("color:white");


    nameEdit = new QLineEdit(this);
    connect(nameEdit,&QLineEdit::textChanged,[&](){
        m_notifyMsg->hide();
        this->setFixedSize(600,400);
    });


    okBtn = new QPushButton("创建",this);
    okBtn->setStyleSheet("QPushButton{border:1px solid #86949e;background-color:#f4f4f4;border-radius:3px}"
                         "QPushButton:hover{background-color:#bee7fd;}"
                         "QPushButton:pressed{background-color:#f4f4f4}");
    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(sltCreate()));

    cancelBtn = new QPushButton("取消",this);
    cancelBtn->setStyleSheet("QPushButton{border:1px solid #86949e;background-color:#f4f4f4;border-radius:3px}"
                             "QPushButton:hover{background-color:#bee7fd;}"
                             "QPushButton:pressed{background-color:#f4f4f4}");
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(close()));

    idLabel = new QLabel(this);
    idLabel->setFont(font2);
    idLabel->setGeometry(80,250,60,30);
    idLabel->setVisible(false);

    welcome->setGeometry(120,60,140,30);
    logo->setGeometry(380,35,80,80);

    nameLabel->setGeometry(140,200,120,30);
    nameEdit->setGeometry(260,203,200,25);
    nameEdit->setStyleSheet("background:transparent;color:white;border:1px solid white;border-style:outset");

    okBtn->setGeometry(320,340,100,30);
    cancelBtn->setGeometry(440,340,100,30);

    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,QBrush(QPixmap(":/Icons/MainWindow/createGroup.png").
                                             scaled(this->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));// 使用平滑的缩放方式
    this->setPalette(palette);// 给widget加上背景图
}

void CreateGroupWnd::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_pressedPoint = event->globalPos() - this->pos();
        m_isPressed = true;
        event->accept();
    }
}


void CreateGroupWnd::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressed = false;
    event->accept();
}

void CreateGroupWnd::mouseMoveEvent(QMouseEvent *event)
{
    if((event->buttons()&(Qt::LeftButton)) && m_isPressed == true){
        move(event->globalPos() - m_pressedPoint);
        event->accept();
    }
}

void CreateGroupWnd::sltCreate()
{
    QString text = nameEdit->text();
    if(text.isEmpty()){
        this->setFixedSize(600,420);
        m_notifyMsg->setText("请输入群名称");
        m_notifyMsg->move(0,400);
        m_notifyMsg->show();
    }else{
        if(text.size() > 20){
            this->setFixedSize(600,420);
            m_notifyMsg->setText("群名称须少于20个字符");
            m_notifyMsg->move(0,400);
            m_notifyMsg->show();
        }else{
            QJsonObject json;
            json.insert("adminID",MyApp::m_nId);
            json.insert("name",text);
            json.insert("time",QDateTime::currentSecsSinceEpoch());

            qDebug() << "创建新群！";
            emit signalCreateGroup(json);
        }
    }
}
