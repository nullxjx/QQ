#ifndef LEFTBAR_H
#define LEFTBAR_H

#include "mybutton.h"
#include "headlabel.h"

#include <QWidget>
#include <QButtonGroup>

class LeftBar : public QWidget
{
    Q_OBJECT
public:
    explicit LeftBar(QWidget *parent = nullptr);

    //MyButton *headIcon;
    HeadLabel *headLabel;

    MyButton *chatList;
    MyButton *contacts;
    MyButton *settings;
    QButtonGroup *m_btnGroup;

protected:
    // 重写鼠标操作以实现移动窗口
    virtual void mousePressEvent(QMouseEvent *event) override;

signals:

public slots:
};

#endif // LEFTBAR_H
