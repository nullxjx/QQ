#include "inputwidget.h"
#include "myapp.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QDateTime>
#include <QActionGroup>
#include <QMenu>

InputWidget::InputWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QStringList tmp;
    tmp << ":/Icons/MainWindow/emoji.png"
        << ":/Icons/MainWindow/emoji2.png"
        << ":/Icons/MainWindow/emoji.png";
    emoji = new MyButton(nullptr,tmp,QSize(25,25),NormalBtn);

    tmp.clear();
    tmp << ":/Icons/MainWindow/picture.png"
        << ":/Icons/MainWindow/picture2.png"
        << ":/Icons/MainWindow/picture.png";
    picture = new MyButton(nullptr,tmp,QSize(25,25),NormalBtn);

    tmp.clear();
    tmp << ":/Icons/MainWindow/file.png"
        << ":/Icons/MainWindow/file2.png"
        << ":/Icons/MainWindow/file.png";
    file = new MyButton(nullptr,tmp,QSize(25,25),NormalBtn);

    tmp.clear();
    tmp << ":/Icons/MainWindow/screenshot.png"
        << ":/Icons/MainWindow/screenshot2.png"
        << ":/Icons/MainWindow/screenshot.png";
    screenshot = new MyButton(nullptr,tmp,QSize(25,25),NormalBtn);


    QHBoxLayout *toolBtnLayout = new QHBoxLayout;
    toolBtnLayout->addWidget(emoji);
    toolBtnLayout->addWidget(picture);
    toolBtnLayout->addWidget(file);
    toolBtnLayout->addWidget(screenshot);
    toolBtnLayout->addStretch(1);
    toolBtnLayout->setSpacing(20);
    toolBtnLayout->setContentsMargins(0,0,0,0);

    QHBoxLayout *bottomLayout = new QHBoxLayout;

    sendBtn = new QPushButton("发送");
    sendBtn->setFixedSize(70,30);

    QMenu *sendMenu = new QMenu(this);
    QAction *actionEnter     = sendMenu->addAction(QIcon(""), tr("按Enter键发送消息"));
    QAction *actionCtrlEnter = sendMenu->addAction(QIcon(""), tr("按Ctrl+Enter键发送消息"));
    connect(sendMenu,SIGNAL(triggered(QAction *)),this,SLOT(saveSendWay(QAction *)));

    actionBtn = new QPushButton(this);
    actionBtn->setFixedSize(20,30);
    actionBtn->setStyleSheet("QPushButton::menu-indicator {image: url(:/Icons/MainWindow/sendBtn_arrow_down.png);"
                             "subcontrol-origin: padding;subcontrol-position: center;}"
                             "QPushButton{border-right:1px solid #e1e1e1;"
                             "border-top:1px solid #e1e1e1;"
                             "border-bottom:1px solid #e1e1e1;}"
                             "QPushButton:hover{background-color:#1296db;}"
                             "QPushButton:pressed{background-color:#1296db}");

    // 设置互斥
    QActionGroup *actionGroup = new QActionGroup(this);
    actionGroup->addAction(actionEnter);
    actionGroup->addAction(actionCtrlEnter);

    // 设置可选
    actionEnter->setCheckable(true);
    actionCtrlEnter->setCheckable(true);

    // 读取配置
    if(MyApp::sendWay)
        actionEnter->setChecked(true);
    else
        actionCtrlEnter->setChecked(true);

    actionBtn->setMenu(sendMenu);

    bottomLayout->addStretch(1);
    bottomLayout->addWidget(sendBtn);
    bottomLayout->addWidget(actionBtn);
    bottomLayout->setSpacing(0);
    bottomLayout->setContentsMargins(0,0,30,5);

    textEdit = new MyTextEdit;
    textEdit->setFixedHeight(130);
    textEdit->setStyleSheet("border: none;background-color:#F0F0F0;");

    mainLayout->addLayout(toolBtnLayout);
    mainLayout->addWidget(textEdit);
    mainLayout->addLayout(bottomLayout);
    mainLayout->setContentsMargins(30,20,30,10);

    setLayout(mainLayout);

    connect(textEdit,SIGNAL(mousePressed()),this,SLOT(changeColor()));
    connect(textEdit,SIGNAL(focusOut()),this,SLOT(restoreColor()));

    setStyleSheet("border: none;background-color:#F0F0F0;");
    setStyleSheet("border-top:1px solid #E6E6E6");

    sendBtn->setStyleSheet("QPushButton{border-left:1px solid #e1e1e1;"
                           "border-top:1px solid #e1e1e1;"
                           "border-bottom:1px solid #e1e1e1;}"
                           "QPushButton:hover{background-color:#1296db;}"
                           "QPushButton:pressed{background-color:#1296db}");
    sendBtn->setFocusPolicy(Qt::NoFocus);
    actionBtn->setFocusPolicy(Qt::NoFocus);
}

void InputWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void InputWidget::changeColor()
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(Qt::white));
    setAutoFillBackground(true);
    setPalette(pal);

    pal.setColor(QPalette::Background, QColor("#F0F0F0"));
    actionBtn->setAutoFillBackground(true);
    actionBtn->setPalette(pal);

    sendBtn->setAutoFillBackground(true);
    sendBtn->setPalette(pal);

    textEdit->setStyleSheet("border: none;background-color:white;");
}

void InputWidget::restoreColor()
{
    setStyleSheet("border: none;background-color:#F0F0F0;");
    setStyleSheet("border-top:1px solid #E6E6E6");
    textEdit->setStyleSheet("border: none;background-color:#F0F0F0;");
}

void InputWidget::saveSendWay(QAction *action)
{
    if(!action->text().compare("按Enter键发送消息")){
        MyApp::sendWay = true;
    }else {
        MyApp::sendWay = false;
    }
    MyApp::saveConfig();
}

