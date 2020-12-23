#ifndef ROTATINGSTACKEDWIDGET_H
#define ROTATINGSTACKEDWIDGET_H

#include "loginwnd.h"
#include "networksetwnd.h"
#include "registerwnd.h"
#include "changepasswordwnd.h"

#include <QStackedWidget>
#include <QMouseEvent>

class RotatingStackedWidget : public QStackedWidget
{
    Q_OBJECT

public:
    explicit RotatingStackedWidget(QWidget *parent = nullptr);
    ~RotatingStackedWidget() override;
    // 初始化旋转的窗口;
    void initRotateWindow();

protected:
    // 重写鼠标操作以实现移动窗口
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent* event) override;

public slots:
    // 开始旋转窗口;
    void onRotateWindow();
    // 窗口旋转结束;
    void onRotateFinished();
    // 最小化窗口;
    void onHideWindow();

    void sltAutoLogin();

    void sltOpenRegisterWnd();
    void sltOpenChangePasswordWnd();
    void sltCloseRegisterWnd(QPoint);
    void sltCloseFindwordWnd(QPoint);

private:
    // 当前窗口是否正在旋转;
    bool m_isRoratingWindow;
    LoginWnd *loginWnd;
    NetWorkSetWnd *netWorkSetWnd;
    int m_nextPageIndex;

    RegisterWnd *registerWnd;
    ChangePasswordWnd *changePasswordWnd;

    QPoint m_pressedPoint; // for moving window
    bool   m_isPressed = false;
};

#endif // ROTATINGSTACKEDWIDGET_H
