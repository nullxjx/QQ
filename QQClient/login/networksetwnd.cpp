#include "networksetwnd.h"
#include "global.h"
#include "qiplineedit.h"

#include <QBitmap>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>


NetWorkSetWnd::NetWorkSetWnd(QWidget *parent)
    : WindowBase(parent)
{
    QFont font = QFont("Microsoft YaHei", 10, 50, false);
    QFont font2 = QFont("Microsoft YaHei", 9, 48, false);

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
    connect(m_menuCloseBtn, &QPushButton::pressed, this, &NetWorkSetWnd::closeWindow);

    m_menuMinBtn = new QPushButton(this);
    m_menuMinBtn->setFlat(true);
    m_menuMinBtn->setFixedSize(30, 32);
    m_menuMinBtn->move(370, 0);
    m_menuMinBtn->setStyleSheet("QPushButton{ border-image: url(:/loginwnd/min_normal); }"
                                "QPushButton:hover:!pressed{ border-image: url(:/loginwnd/min_hover); }"
                                "QPushButton:hover:pressed{ border-image: url(:/loginwnd/min_press); border-style:none; }");
    connect(m_menuMinBtn, &QPushButton::pressed, this, &NetWorkSetWnd::hideWindow);

    titleLabel = new QLabel("服务器设置",this);
    titleLabel->setFont(font);
    titleLabel->move(40,60);

    ipAddrLabel = new QLabel("服务器IP地址：",this);
    ipAddrLabel->setFixedSize(150,25);
    ipAddrLabel->setFont(font2);
    ipAddrLabel->move(40,120);

#if 0

    ipEdit = new IPLineEdit(this);
    ipEdit->move(170,120);
    ipEdit->setFixedSize(200,25);


    QLineEdit *ipEdit_ = new QLineEdit(this);
    ipEdit_->move(170,120);
    ipEdit_->setFixedSize(200,25);
#endif

    ipEdit = new QIPLineEdit(this);
    ipEdit->move(170,120);
    ipEdit->setFixedSize(200,25);
    connect(ipEdit, SIGNAL(textChanged()), this, SLOT(hideErrorMsg()));

    msgPortLabel = new QLabel("消息服务器端口：",this);
    msgPortLabel->setFixedSize(150,25);
    msgPortLabel->setFont(font2);
    msgPortLabel->move(40,160);

    msgPortEdit = new QLineEdit(this);
    msgPortEdit->setFixedSize(100,25);
    msgPortEdit->move(170,160);
    msgPortEdit->setStyleSheet("background:transparent;border-width:1px;border-style:outset");
    connect(msgPortEdit, SIGNAL(textChanged(const QString &)), this, SLOT(hideErrorMsg()));

    filePortLabel = new QLabel("文件服务器端口：",this);
    filePortLabel->setFixedSize(150,25);
    filePortLabel->setFont(font2);
    filePortLabel->move(40,200);

    filePortEdit = new QLineEdit(this);
    filePortEdit->setFixedSize(100,25);
    filePortEdit->move(170,200);
    filePortEdit->setStyleSheet("background:transparent;border-width:1px;border-style:outset");
    connect(filePortEdit, SIGNAL(textChanged(const QString &)), this, SLOT(hideErrorMsg()));

    okBtn = new QPushButton("确定",this);
    okBtn->setFont(font2);
    okBtn->move(180,270);
    okBtn->setFixedSize(100,30);
    okBtn->setStyleSheet("QPushButton{border:1px solid #86949e;background-color:#f4f4f4;border-radius:3px}"
                         "QPushButton:hover{background-color:#bee7fd;}"
                         "QPushButton:pressed{background-color:#f4f4f4}");
    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(btnClicked()));

    cancelBtn = new QPushButton("取消",this);
    cancelBtn->setFont(font2);
    cancelBtn->move(300,270);
    cancelBtn->setFixedSize(100,30);
    cancelBtn->setStyleSheet("QPushButton{border:1px solid #86949e;background-color:#f4f4f4;border-radius:3px}"
                             "QPushButton:hover{background-color:#bee7fd;}"
                             "QPushButton:pressed{background-color:#f4f4f4}");
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(btnClicked()));

    readConfig();
}

void NetWorkSetWnd::readConfig()
{
    if(!MyApp::m_strHostAddr.isEmpty())
        ipEdit->setText(MyApp::m_strHostAddr);
    if(MyApp::m_nMsgPort != 0)
        msgPortEdit->setText(QString::number(MyApp::m_nMsgPort));
    if(MyApp::m_nFilePort != 0)
        filePortEdit->setText(QString::number(MyApp::m_nFilePort));
}

void NetWorkSetWnd::showErrorMsg(QString msg)
{
    this->setFixedSize(430,350);
    m_notifyMsg->setText("  " + msg);
    m_notifyMsg->move(0,330);
    m_notifyMsg->show();
}

void NetWorkSetWnd::hideErrorMsg()
{
    this->setFixedSize(430,330);
    m_notifyMsg->hide();
}

void NetWorkSetWnd::btnClicked()
{
    if(sender() == okBtn){
        //保存配置

        QString hostIPAddr = ipEdit->getText();
        if(!myHelper::IsIP(hostIPAddr)){
            //QMessageBox::information(this,"错误","IP地址设置错误!");
            showErrorMsg("IP地址设置错误!");
            return;
        }

        int msgPort = msgPortEdit->text().toInt();
        if (msgPort > 65535 || msgPort < 100) {
            //QMessageBox::information(this,"错误","消息端口设置错误!");
            showErrorMsg("消息端口设置错误!");
            return;
        }

        int filePort = filePortEdit->text().toInt();
        if (filePort > 65535 || filePort < 100) {
            //QMessageBox::information(this,"错误","文件端口设置错误!");
            showErrorMsg("文件端口设置错误!");
            return;
        }

        if(msgPort == filePort){
            //QMessageBox::information(this,"错误","2个端口不能相同!");
            showErrorMsg("2个端口不能相同!");
            return;
        }

        MyApp::m_strHostAddr = hostIPAddr;
        MyApp::m_nMsgPort = msgPort;
        MyApp::m_nFilePort = filePort;

        MyApp::saveConfig();
    }

    emit rotateWindow();
}
