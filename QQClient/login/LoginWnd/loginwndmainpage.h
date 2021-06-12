/****************************************************************************
** QQ-Like 登录界面窗体: 登录界面显示的控件
****************************************************************************/
#ifndef LOGINWNDMAINPAGE_H
#define LOGINWNDMAINPAGE_H

#include "headlabel.h"
#include "checkbox.h"
#include "clickablelabel.h"
#include "loginlineedit.h"
#include "windowbase.h"

#include <QCheckBox>
#include <QPushButton>


class LoginWndMainPage : public WindowBase
{
    Q_OBJECT
public:
    explicit LoginWndMainPage(QWidget *parent = nullptr);
    QString getID(){return m_userIDEdit->text();}
    QString getPassword(){return  m_passwordEdit->text();}
    void readConfig();

protected:
    virtual void paintEvent(QPaintEvent *) override;
    virtual void showEvent(QShowEvent*) override;

signals:
    void loginSignal();
    void closeWindow();
    void hideWindow();
    void rotateWindow();

    void hideNotifyMsg();
    void showNotifyMsg(QString);

    void openRegisterWnd();
    void openChangePasswordWnd();
    void changeLoginingHead();

public slots:
    void openRegisterAccountUrl(ClickableLabel*);
    void openFindPasswordUrl(ClickableLabel*);
    void autoLoginChecked(bool isChecked);
    void loginBtnClicked();
    void remeberPassword(bool isChecked);
    void checkAutoLogin();
    void changeHead();

private:
    QPushButton* m_memuSetBtn;           //网络设置按钮
    QPushButton* m_menuCloseBtn;         // 菜单栏关闭按钮
    QPushButton* m_menuMinBtn;           // 菜单栏最小化按钮
    HeadLabel*   m_headLabel;            // 头像

    LoginLineEdit*  m_userIDEdit;       // 用户名
    LoginLineEdit*  m_passwordEdit;     // 密码
    CheckBox*       m_autoLogin;        // 自动登录
    CheckBox*       m_remeberPassword;  // 自动登录
    QPushButton*    m_loginBtn;         // 登录按钮

    ClickableLabel* m_findPassword;     // 找回密码
    ClickableLabel* m_registerAccount;  // 注册账号
};


#endif // LOGINWNDMAINPAGE_H
