#ifndef BUBBLEVIEW_H
#define BUBBLEVIEW_H

#include "bubbleinfo.h"
#include "mybutton.h"
#include "radiusprogressbar.h"

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QLabel>
#include <QMovie>
#include <QProgressBar>
#include <QMenu>

class BubbleView : public QWidget
{
    Q_OBJECT
public:
    explicit BubbleView(QWidget *parent = nullptr,BubbleInfo* info = nullptr);

    QSize setRect();
    QSize getRealString(QString src);

    void setPopMenu(QMenu *menu){popMenu = menu;}
    QString getFileSize(qint64);

    BubbleInfo *info;

public slots:
    void slotupdateProgressBar(qint64,qint64);
    void sltUpdatePopMenu();
    void sltHideAnimation();

private:

    int height;//整个widget的高度
    int textWidth;
    int lineHeight;

    QFont textFont;
    QFont sysMsgFont;//系统消息字体

    QRect iconRect;
    QRect triangleRect;//气泡中的小三角
    QRect frameRect;
    QRect textRect;

    MyButton *headIcon;
    QLabel *loadingLabel;
    QMovie *loadingMovie;

    //QProgressBar *progressBar;
    RadiusProgressBar *progressBar;

    QLabel *fileNameLabel;
    QLabel *fileIconLabel;
    QLabel *fileSizeLabel;
    QLabel *picture;
    QLabel *downloadLabel;

    QLabel *errorLabel;

    QMenu *popMenu;

protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

signals:
    void mouseRightClicked(BubbleInfo*,QMenu*);
};

#endif // BUBBLEVIEW_H
