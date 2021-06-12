#include "settingswnd.h"
#include "titlebar.h"
#include "myapp.h"

#include <QDebug>

SettingsWnd::SettingsWnd(QWidget *parent) : QWidget(parent)
{
    setFixedSize(700,500);
    setWindowFlags(Qt::WindowCloseButtonHint);
    //this->setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("设置");
    setStyleSheet("background-color:white");

    QFont font = QFont("Microsoft YaHei", 15, 50, false);
    autoLoginLabel = new QLabel("自动登陆",this);
    autoLoginLabel->setFont(font);
    autoLoginLabel->setGeometry(40,40,100,30);
    autoLoginCheckBox = new QCheckBox(this);
    autoLoginCheckBox->setGeometry(150,43,30,30);

    bool checked = MyApp::autoLogin;
    autoLoginCheckBox->setChecked(checked);

    connect(autoLoginCheckBox,SIGNAL(stateChanged(int)),
            this,SLOT(setAutoLogin(int)));
}

void SettingsWnd::setAutoLogin(int state)
{
    qDebug() << "check state:" << state;
    MyApp::autoLogin = state;
    MyApp::saveConfig();
}
