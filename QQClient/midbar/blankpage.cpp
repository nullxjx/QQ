#include "blankpage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>

#ifdef Q_OS_WIN
#pragma comment(lib, "user32.lib")
#include <qt_windows.h>
#endif

BlankPage::BlankPage(QWidget *parent) : QWidget(parent)
{
    setWindowFlag(Qt::FramelessWindowHint);
    //#ebeae8
    setStyleSheet("border:none;background-color:#ebeae8;");
    QVBoxLayout *layout = new QVBoxLayout(this);
    QFont font("Microsoft YaHei", 10, 40, false);
    QLabel *label = new QLabel();
    label->setPixmap(QPixmap(":/Icons/MainWindow/blank.png").
                     scaled(200,200,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QLabel *label2 = new QLabel(tr("打开联系人列表选择好友或者群聊天"),this);
    label2->setFont(font);
    label2->setFixedSize(300,30);
    label2->setStyleSheet("color:#1296db");
    label2->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QLabel *label3 = new QLabel(this);
    label3->setPixmap(QPixmap(":/Icons/MainWindow/left_arrow.png").
                      scaled(30,30,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    label3->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    label3->move(0,115);

    label2->move(25,150);

    layout->setSpacing(5);
    layout->setContentsMargins(0,0,0,0);
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    layout->addWidget(label);
}

void BlankPage::mousePressEvent(QMouseEvent *event)
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
