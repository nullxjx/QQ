/****************************************************************************
** QQ-Like 登录界面窗体: 登录界面显示的控件
****************************************************************************/
#include "loginwndmainpage.h"
#include "myapp.h"
#include "global.h"

#include <QDesktopServices>
#include <QUrl>
#include <QDateTime>
#include <QDebug>
#include <QPainter>

LoginWndMainPage::LoginWndMainPage(QWidget *parent): WindowBase(parent)
{
    this->setFixedSize(430,330);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    m_menuCloseBtn = new QPushButton(this);
    m_menuCloseBtn->setFlat(true);
    m_menuCloseBtn->setFixedSize(30, 32);
    m_menuCloseBtn->move(400, 0);
    m_menuCloseBtn->setStyleSheet("QPushButton{ border-image: url(:/loginwnd/close_normal); }"
                                  "QPushButton:hover:!pressed{ border-image: url(:/loginwnd/close_hover); }"
                                  "QPushButton:hover:pressed{ border-image: url(:/loginwnd/close_press); "
                                  "border-style:none; }");
    connect(m_menuCloseBtn,SIGNAL(clicked(bool)),this,SIGNAL(closeWindow()));

    m_menuMinBtn = new QPushButton(this);
    m_menuMinBtn->setFlat(true);
    m_menuMinBtn->setFixedSize(30, 32);
    m_menuMinBtn->move(370, 0);
    m_menuMinBtn->setStyleSheet("QPushButton{ border-image: url(:/loginwnd/min_normal); }"
                                "QPushButton:hover:!pressed{ border-image: url(:/loginwnd/min_hover); }"
                                "QPushButton:hover:pressed{ border-image: url(:/loginwnd/min_press); "
                                "border-style:none; }");
    connect(m_menuMinBtn,SIGNAL(clicked(bool)),this,SIGNAL(hideWindow()));

    m_memuSetBtn = new QPushButton(this);
    m_memuSetBtn->setFlat(true);
    m_memuSetBtn->setFixedSize(30, 32);
    m_memuSetBtn->move(340, 0);
    m_memuSetBtn->setStyleSheet("QPushButton{ border-image: url(:/loginwnd/set_normal); }"
                                "QPushButton:hover:!pressed{ border-image: url(:/loginwnd/set_hover); }"
                                "QPushButton:hover:pressed{ border-image: url(:/loginwnd/set_press); "
                                "border-style:none; }");
    connect(m_memuSetBtn,SIGNAL(clicked(bool)),this,SIGNAL(rotateWindow()));

    QString headPath = MyApp::m_strHeadPath + MyApp::m_strHeadFile;
    //qDebug() << headPath;
    QFileInfo fileInfo(headPath);
    if(!fileInfo.exists() || MyApp::m_strHeadFile.isEmpty()){
        headPath = ":/loginwnd/defalut_head";
    }

    m_headLabel = new HeadLabel(this, headPath, ":/loginwnd/head_bkg_shadow", ":/loginwnd/head_bkg_highlight");
    m_headLabel->setFixedSize(98, 98);
    m_headLabel->move(166, 75);

    m_userIDEdit = new LoginLineEdit(this);
    m_userIDEdit->setBottomIcon(":/loginwnd/edit_frame_normal", ":/loginwnd/edit_frame_hover");

    m_userIDEdit->setLeftIcon(":/loginwnd/qqnum_normal", ":/loginwnd/qqnum_hover");
    m_userIDEdit->setPlaceholderText(tr("QQ号码"));
    m_userIDEdit->setStyleSheet("font-size: 14px;font-family:Microsoft YaHei;");
    m_userIDEdit->move(100, 170);
    //connect(m_userIDEdit,SIGNAL(textChanged()),this,SIGNAL(hideNotifyMsg()));
    connect(m_userIDEdit,SIGNAL(textChanged()),this,SLOT(changeHead()));

    m_passwordEdit = new LoginLineEdit(this);
    m_passwordEdit->setBottomIcon(":/loginwnd/edit_frame_normal", ":/loginwnd/edit_frame_hover");
    m_passwordEdit->setLeftIcon(":/loginwnd/psw_normal", ":/loginwnd/psw_hover");
    m_passwordEdit->setPlaceholderText(tr("密码"));
    m_passwordEdit->setStyleSheet("font-size: 14px;font-family:Microsoft YaHei;");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->move(100, 210);
    connect(m_passwordEdit, SIGNAL(textChanged()), this, SIGNAL(hideNotifyMsg()));
    connect(m_passwordEdit, SIGNAL(returnPressed()), this, SLOT(loginBtnClicked()));

    m_autoLogin = new CheckBox(false, this);
    m_autoLogin->move(98, 248);
    m_autoLogin->setText(tr("自动登录"));
    connect(m_autoLogin, &CheckBox::statusChanged, this, &LoginWndMainPage::autoLoginChecked);

    m_remeberPassword = new CheckBox(false, this);
    m_remeberPassword->move(261, 248);//(192, 248)
    m_remeberPassword->setText(tr("记住密码"));
    connect(m_remeberPassword,&CheckBox::statusChanged,this,&LoginWndMainPage::remeberPassword);


    m_findPassword = new ClickableLabel(this);
    m_findPassword->setText(tr("修改密码"));
    m_findPassword->setStyleSheet("font-size: 12px;font-family:Microsoft YaHei; color: #a6a6a6");
    m_findPassword->setFixedSize(55, 20);
    m_findPassword->move(370, 300);//(280, 248)
    connect(m_findPassword, &ClickableLabel::clickSignal, this, &LoginWndMainPage::openFindPasswordUrl);

    m_registerAccount = new ClickableLabel(this);
    m_registerAccount->setText(tr("注册账号"));
    m_registerAccount->setStyleSheet("font-size: 12px;font-family:Microsoft YaHei; color: #a6a6a6;");
    m_registerAccount->setFixedSize(55, 20);
    m_registerAccount->move(12, 300);
    connect(m_registerAccount, &ClickableLabel::clickSignal, this, &LoginWndMainPage::openRegisterAccountUrl);

    m_loginBtn = new QPushButton(this);
    m_loginBtn->setFixedSize(230, 35);
    m_loginBtn->move(96, 277);
    m_loginBtn->setStyleSheet("QPushButton{ border-image: url(:/loginwnd/button_login_normal); "
                              "font-size: 15px;font-family:Microsoft YaHei; color: #ffffff}"
                              "QPushButton:hover:!pressed{ border-image: url(:/loginwnd/button_login_hover); }"
                              "QPushButton:hover:pressed{ border-image: url(:/loginwnd/button_login_down); "
                              "border-style:none; }");
    m_loginBtn->setText(tr("登录"));
    connect(m_loginBtn, &QPushButton::clicked, this, &LoginWndMainPage::loginBtnClicked);

    readConfig();
}

void LoginWndMainPage::readConfig()
{
    if(MyApp::checked){
        m_userIDEdit->setText(QString::number(MyApp::m_nId));
        m_passwordEdit->setText(MyApp::m_strPassword);
        m_remeberPassword->changeStatus(true);
    }

    if(MyApp::autoLogin){
        m_autoLogin->changeStatus(true);
        m_loginBtn->setEnabled(false);
    }
}

void LoginWndMainPage::paintEvent(QPaintEvent *e)
{
    WindowBase::paintEvent(e);

    QPainter painter(this);
    painter.drawPixmap(0, 0, QPixmap(":/loginwnd/logo-QQ"));
    painter.drawPixmap(0, 130, QPixmap(":/loginwnd/bk_shadow"));
}

void LoginWndMainPage::showEvent(QShowEvent *e)
{
    m_passwordEdit->setFocus();
    return WindowBase::showEvent(e);
}

void LoginWndMainPage::openRegisterAccountUrl(ClickableLabel *)
{
    //QDesktopServices::openUrl(QUrl(QLatin1String("https://baidu.com")));
    emit openRegisterWnd();
}

void LoginWndMainPage::openFindPasswordUrl(ClickableLabel *)
{
    //QDesktopServices::openUrl(QUrl(QLatin1String("https://baidu.com")));
    emit openChangePasswordWnd();
}

void LoginWndMainPage::remeberPassword(bool isChecked)
{
    MyApp::checked = isChecked;
    MyApp::saveConfig();
}

void LoginWndMainPage::checkAutoLogin()
{
    if(MyApp::autoLogin){
        qDebug() << "begin to autoLogin...";
        myHelper::printLogFile("begin to autoLogin...");

        loginBtnClicked();
    }
}

void LoginWndMainPage::changeHead()
{
    hideNotifyMsg();
    QString text = m_userIDEdit->text();
    int cnt = text.size();
    for(int i = 0;i < cnt;i++){
        if(text[i] >= 48 && text[i] <= 57)
            continue;
        else
            return;
    }

    QString headPath = MyApp::m_strHeadPath + QString::number(text.toInt()) + ".png";
    QFileInfo fileInfo(headPath);
    if(fileInfo.exists()){
        MyApp::m_strHeadFile = QString::number(text.toInt()) + ".png";
        m_headLabel->changeHead(headPath);
        //qDebug() << "update head:" << headPath;
    }else{
        MyApp::m_strHeadFile = "";
        headPath = ":/loginwnd/defalut_head";
        m_headLabel->changeHead(headPath);
        //qDebug() << "update head:" << headPath;
    }

    emit changeLoginingHead();
}

void LoginWndMainPage::autoLoginChecked(bool isChecked)
{
    if(isChecked == true){
        m_remeberPassword->changeStatus(true);
        MyApp::checked = true;
        MyApp::autoLogin = true;
        MyApp::saveConfig();
    }else{
        MyApp::autoLogin = false;
        MyApp::saveConfig();
    }
}

void LoginWndMainPage::loginBtnClicked()
{
    if(m_userIDEdit->text().isEmpty()){
        emit showNotifyMsg(tr("注意: 请您输入用户名后再登录"));
    }else if(m_passwordEdit->text().isEmpty()){
        emit showNotifyMsg(tr("注意: 请您输入密码后再登录"));
    }else{
        qDebug() << "login..." << endl;
        myHelper::printLogFile("login...\n");
        emit loginSignal();
    }
}
