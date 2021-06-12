#include "defaultpage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>

#ifdef Q_OS_WIN
#pragma comment(lib, "user32.lib")
#include <qt_windows.h>
#endif

DefaultPage::DefaultPage(QWidget *parent) : QWidget(parent)
{
    setWindowFlag(Qt::FramelessWindowHint);
    setStyleSheet("border:none;background-color:#F0F0F0;");
    QVBoxLayout *layout = new QVBoxLayout(this);
    QFont font("Microsoft YaHei", 15, 40, false);
    QFont font2("Microsoft YaHei", 10, 10, false);

    QLabel *label = new QLabel();
    label->setPixmap(QPixmap(":/Icons/MainWindow/title.png"));
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QLabel *label2 = new QLabel(tr("打开世界的另一扇窗"));
    label2->setFont(font);
    label2->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QLabel *label3 = new QLabel(tr("主动一点，世界会更大"));
    label3->setFont(font2);
    label3->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    layout->setSpacing(5);
    layout->setContentsMargins(0,0,0,0);
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    layout->addStretch(2);
    layout->addWidget(label);
    layout->addSpacing(30);
    layout->addWidget(label2);
    layout->addWidget(label3);
    layout->addStretch(3);
}

void DefaultPage::mousePressEvent(QMouseEvent *event)
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
