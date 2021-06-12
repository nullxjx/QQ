/****************************************************************************
** QQ-Like 登录窗口登录中界面
****************************************************************************/
#ifndef LOGINWNDLOGININGPAGE_H
#define LOGINWNDLOGININGPAGE_H

#include "windowbase.h"
#include "headlabel.h"

#include <QLabel>
#include <QPushButton>


class LoginWndLoginingPage : public WindowBase
{
    Q_OBJECT
public:
    explicit LoginWndLoginingPage(QWidget *parent = nullptr);

protected:
    virtual void hideEvent(QHideEvent *) override;

signals:
    void cancelSignal();
    void closeWindow();
    void hideWindow();
    void animationFinished();


public slots:
    void changePoints();
    void cancelButtonClicked(bool);
    void showMainUI();
    void loginSuccess();

    void updateHead();

private:
    QLabel*          m_loginingLabel;    // 正在登录中标签
    QPushButton*     m_cancelBtn;        // 取消登录按钮
    QTimer*          m_loginingTimer;    // 登录中动画计时器
    QString          m_points;           // 登录中后面的小点

    HeadLabel*   m_headLabel;            // 头像
};


#endif // LOGINWNDLOGININGPAGE_H
