#include "titlebar.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QPoint>
#include <QtDebug>
#include <QPalette>

#ifdef Q_OS_WIN
#pragma comment(lib, "user32.lib")
#include <qt_windows.h>
#endif

TitleBar::TitleBar(QWidget *parent, int tag)
    : QWidget(parent),tag(tag)
{
    barHeight = 30;
    setWindowFlag(Qt::FramelessWindowHint);
    setFixedHeight(barHeight);
    setFocusPolicy(Qt::StrongFocus);

    if(tag == 2){
        iconLabel = new QLabel(this);
        titleLabel = new QLabel(this);
        topBtn = new TitleButton(this,1,":/Icons/MainWindow/top.png");
        minBtn = new TitleButton(this,1,":/Icons/MainWindow/min.png");
        maxBtn = new TitleButton(this,1,":/Icons/MainWindow/max.png");
        closeBtn = new TitleButton(this,0,":/Icons/MainWindow/close.png");
        topBtn->setToolTip("Stay on top");
        minBtn->setToolTip("Minimize");
        maxBtn->setToolTip("Maximize");
        closeBtn->setToolTip("Close");

        QHBoxLayout *pLayout = new QHBoxLayout(this);
        pLayout->addWidget(iconLabel);
        pLayout->addSpacing(5);
        pLayout->addWidget(titleLabel);
        pLayout->addWidget(topBtn);
        pLayout->addWidget(minBtn);
        pLayout->addWidget(maxBtn);
        pLayout->addWidget(closeBtn);
        pLayout->setSpacing(0);
        pLayout->setContentsMargins(0, 0, 0, 0);
        setLayout(pLayout);

        connect(topBtn, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
        connect(minBtn, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
        connect(maxBtn, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
        connect(closeBtn, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    }else{
        maxBtn = new TitleButton(this,1,":/Icons/MainWindow/max.png");
        maxBtn->setVisible(false);
        connect(maxBtn, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    }
}


TitleBar::~TitleBar()
{

}

void TitleBar::setTitleText(QString text)
{
    titleLabel->setText(text);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    qDebug() << "tilteBar DoubleClicked...";

    emit maxBtn->clicked();
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
#ifdef Q_OS_WIN
    if (ReleaseCapture()){
        QWidget *pWindow = this->window();
        if (pWindow->isTopLevel()){
            SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
        }
    }
    event->ignore();
#endif
}

bool TitleBar::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::WindowTitleChange:
    {
        QWidget *pWidget = qobject_cast<QWidget *>(obj);
        if (pWidget){
            titleLabel->setText(pWidget->windowTitle());
            return true;
        }
    }
        break;
    case QEvent::WindowIconChange:
    {
        QWidget *pWidget = qobject_cast<QWidget *>(obj);
        if (pWidget){
            QIcon icon = pWidget->windowIcon();
            iconLabel->setPixmap(icon.pixmap(iconLabel->size()));
            return true;
        }
    }
        break;
    case QEvent::WindowStateChange:
    case QEvent::Resize:
        updateMaximize();
        return true;
    default:
        return QWidget::eventFilter(obj, event);
    }
    return QWidget::eventFilter(obj, event);
}

void TitleBar::onClicked()
{
    QPushButton *pButton = qobject_cast<QPushButton *>(sender());

    QWidget *pWindow = this->window();
    if (pWindow->isTopLevel())
    {
        if (pButton == minBtn){
            pWindow->showMinimized();
        }else if (pButton == maxBtn){
            if(pWindow->isMaximized()){
                pWindow->showNormal();//还原
                maxBtn->setIcon(QPixmap(":/Icons/MainWindow/max.png").
                                scaled(15,15,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
            }else{
                pWindow->showMaximized();//最大化
                maxBtn->setIcon(QPixmap(":/Icons/MainWindow/back.png").
                                scaled(15,15,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
            }
        }else if (pButton == closeBtn){
            pWindow->close();
        }else if (pButton == topBtn){
            if(!isOnTop){//当前为非置顶状态，设置为置顶状态
                topBtn->setToolTip("Cancel on Top");
                topBtn->setIcon(QPixmap(":/Icons/MainWindow/top2.png").
                                scaled(15,15,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
            }else{//当前为置顶状态，取消置顶状态
                topBtn->setToolTip("Stay on Top");
                topBtn->setIcon(QPixmap(":/Icons/MainWindow/top.png").
                                scaled(15,15,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
            }

            isOnTop = !isOnTop;
            emit stayOnTop(isOnTop);
        }
    }
}

void TitleBar::updateMaximize()
{
    QWidget *pWindow = this->window();
    if (pWindow->isTopLevel())
    {
        bool bMaximize = pWindow->isMaximized();
        if (bMaximize)
        {
            maxBtn->setToolTip(tr("Restore"));
            maxBtn->setProperty("maximizeProperty", "restore");
        }
        else
        {
            maxBtn->setProperty("maximizeProperty", "maximize");
            maxBtn->setToolTip(tr("Maximize"));
        }

        maxBtn->setStyle(QApplication::style());
    }
}
