#ifndef TITLEBAR_H
#define TITLEBAR_H


#include <QWidget>
#include "titlebutton.h"

class QLabel;
class QPushButton;

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr,int tag = 2);//tag == 0左边，tag == 1中间，tag == 2右边
    ~TitleBar();
    void setTitleText(QString text);

signals:
    void stayOnTop(bool);

protected:

    // 双击标题栏进行界面的最大化/还原
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

    // 进行鼠界面的拖动
    virtual void mousePressEvent(QMouseEvent *event);

    // 设置界面标题与图标
    virtual bool eventFilter(QObject *obj, QEvent *event);

private slots:

    // 进行最小化、最大化/还原、关闭操作
    void onClicked();

private:

    // 最大化/还原
    void updateMaximize();

private:
    QLabel *iconLabel;
    QLabel *titleLabel;
    TitleButton *topBtn;
    TitleButton *minBtn;
    TitleButton *maxBtn;
    TitleButton *closeBtn;

    int barHeight;
    int tag;
    bool isOnTop = false;
};


#endif // TITLEBAR_H
