/****************************************************************************
** QQ-Like 登录窗口登录中界面
****************************************************************************/
#include "loginwndloginingpage.h"
#include "myapp.h"

#include <QDateTime>
#include <QDebug>
#include <QFileInfo>

LoginWndLoginingPage::LoginWndLoginingPage(QWidget *parent) : WindowBase(parent)
{
    this->setFixedSize(430,330);//(430,330)
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    QString headPath = MyApp::m_strHeadPath + MyApp::m_strHeadFile;
    //qDebug() << "headPath:" << headPath;
    QFileInfo fileInfo(headPath);
    if(!fileInfo.exists() || MyApp::m_strHeadFile.isEmpty())
        headPath = ":/loginwnd/defalut_head";


    m_headLabel = new HeadLabel(this, headPath, ":/loginwnd/head_bkg_shadow", ":/loginwnd/head_bkg_highlight");
    m_headLabel->setFixedSize(98, 98);
    m_headLabel->move(166, 90);

    m_loginingLabel = new QLabel(this);
    m_loginingLabel->setText(tr("登录中")+m_points);
    m_loginingLabel->move(168, 175);
    m_loginingLabel->setFixedSize(100, 35);
    m_loginingLabel->setAlignment(Qt::AlignCenter);
    m_loginingLabel->setStyleSheet("font-size: 18px;font-family:Microsoft YaHei; color: #ffffff");

    m_cancelBtn = new QPushButton(this);
    m_cancelBtn->move(168, 270);
    m_cancelBtn->setFixedSize(100, 35);
    m_cancelBtn->setText(tr("取消"));
    m_cancelBtn->setStyleSheet("QPushButton{ background-color:rgba(49,195,255,45);border-color:#96d5fd; "
                               "border-width:1px; border-radius:2px; border-style: solid;"
                               "font-size: 16px;font-family:Microsoft YaHei; color: #ffffff}"
                               "QPushButton:hover:!pressed{ background-color:rgba(111,221,255,45);"
                               "border-color:#96d5fd; border-width:1px; border-radius:2px; border-style: solid;}"
                               "QPushButton:hover:pressed{ background-color:rgba(147,238,255,45);border-color:#96d5fd;"
                               "border-width:1px; border-radius:2px; border-style: solid;}");
    QObject::connect(m_cancelBtn, &QPushButton::clicked, this, &LoginWndLoginingPage::cancelButtonClicked);

    m_loginingTimer = new QTimer(this);
    QObject::connect(m_loginingTimer, &QTimer::timeout, this, &LoginWndLoginingPage::changePoints);
    m_loginingTimer->start(1000);
}

void LoginWndLoginingPage::hideEvent(QHideEvent *)
{
    m_loginingTimer->stop();
    emit hideWindow();
}

void LoginWndLoginingPage::changePoints()
{
    m_points.append('.');
    if(m_points.size() > 3){
        m_points.clear();
    }
    m_loginingLabel->setText(tr("登录中")+m_points);
}

void LoginWndLoginingPage::cancelButtonClicked(bool)
{
    emit cancelSignal();
}

void LoginWndLoginingPage::loginSuccess()
{
    m_loginingTimer->stop();
    m_loginingLabel->setText(tr("登录成功！"));
    //m_headLabel->updateHead();
    QTimer::singleShot(1000,this,SLOT(showMainUI()));
}

void LoginWndLoginingPage::updateHead()
{
    QString headPath = MyApp::m_strHeadPath + MyApp::m_strHeadFile;
    //qDebug() << "headPath:" << headPath;
    QFileInfo fileInfo(headPath);
    if(!fileInfo.exists() || MyApp::m_strHeadFile.isEmpty())
        headPath = ":/loginwnd/defalut_head";

    m_headLabel->changeHead(headPath);
}

void LoginWndLoginingPage::showMainUI()
{
    emit animationFinished();
}
