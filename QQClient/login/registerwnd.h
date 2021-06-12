#ifndef REGISTERWND_H
#define REGISTERWND_H

#include "windowbase.h"

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QMouseEvent>
#include <QJsonValue>

class RegisterWndPrivate : public WindowBase
{
    Q_OBJECT

public:
    RegisterWndPrivate(QWidget *parent = nullptr);

signals:
    void closeWindow();
    void signalRegister(const QJsonValue &jsonVal);

private:
    QLabel* m_notifyMsg;
    QPushButton* m_menuCloseBtn;         // 菜单栏关闭按钮
    QPushButton* m_menuMinBtn;           // 菜单栏最小化按钮
    QLabel *welcome;
    QLabel *logo;
    QLabel *nameLabel;
    QLabel *passwordLabel;
    QLineEdit *nameEdit;
    QLineEdit *passwordEdit;
    QPushButton *okBtn;
    QPushButton *cancelBtn;
    QLabel *idLabel;

public slots:
    void sltBtnClicked();
    void closeWnd();
    void sltRegisterOK(const QJsonValue&);
};


class RegisterWnd : public QWidget
{
    Q_OBJECT

public:
    RegisterWnd(QWidget *parent = nullptr);

signals:
    void closeWindow(QPoint);
    void signalRegister(const QJsonValue &jsonVal);

public slots:
    void sltCloseWnd();
    void sltRegisterOK(const QJsonValue&);

private:
    RegisterWndPrivate *mainWnd;
    QPoint m_pressedPoint; // for moving window
    bool   m_isPressed = false;

protected:
    // 重写鼠标操作以实现移动窗口
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // REGISTERWND_H
