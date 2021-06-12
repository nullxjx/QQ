#ifndef FINDPASSWORDWND_H
#define FINDPASSWORDWND_H

#include "windowbase.h"

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QMouseEvent>
#include <QJsonValue>

class ChangePasswordWndPrivate : public WindowBase
{
    Q_OBJECT

public:
    ChangePasswordWndPrivate(QWidget *parent = nullptr);

signals:
    void closeWindow();
    void signalChangePwd(const QJsonValue &jsonVal);

private:
    QLabel* m_notifyMsg;
    QPushButton* m_menuCloseBtn;         // 菜单栏关闭按钮
    QPushButton* m_menuMinBtn;           // 菜单栏最小化按钮
    QLabel *title;
    QLabel *logo;
    QLabel *idLabel;
    QLabel *oldPasswordLabel;
    QLabel *newPasswordLabel;
    QLineEdit *idEdit;
    QLineEdit *oldPasswordEdit;
    QLineEdit *newPasswordEdit;
    QPushButton *okBtn;
    QPushButton *cancelBtn;

public slots:
    void closeWnd();
    void sltOKBtnClicked();
    void sltChangePwdReply(const QJsonValue &);
};


class ChangePasswordWnd : public QWidget
{
    Q_OBJECT

public:
    ChangePasswordWnd(QWidget *parent = nullptr);

signals:
    void closeWindow(QPoint);
    void signalChangePwd(const QJsonValue &jsonVal);

public slots:
    void sltChangePwdReply(const QJsonValue &);
    void sltCloseWnd();

private:
    ChangePasswordWndPrivate *mainWnd;

    QPoint m_pressedPoint; // for moving window
    bool   m_isPressed = false;

protected:
    // 重写鼠标操作以实现移动窗口
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

};

#endif // FINDPASSWORDWND_H
