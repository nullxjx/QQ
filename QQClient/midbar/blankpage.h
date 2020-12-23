#ifndef BLANKPAGE_H
#define BLANKPAGE_H

#include <QWidget>

class BlankPage : public QWidget
{
    Q_OBJECT
public:
    BlankPage(QWidget *parent = nullptr);

protected:
    // 重写鼠标操作以实现移动窗口
    virtual void mousePressEvent(QMouseEvent *event) override;

signals:

public slots:

};

#endif // BLANKPAGE_H
