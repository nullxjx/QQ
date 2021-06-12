#ifndef CREATEGROUPWND_H
#define CREATEGROUPWND_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QMouseEvent>
#include <QJsonValue>

class CreateGroupWnd : public QDialog
{
    Q_OBJECT

public:
    CreateGroupWnd();

signals:
    void signalCreateGroup(const QJsonValue &jsonVal);

protected:
    // 重写鼠标操作以实现移动窗口
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

private:
    QLabel* m_notifyMsg;
    QPushButton* m_menuCloseBtn;         // 菜单栏关闭按钮
    QPushButton* m_menuMinBtn;           // 菜单栏最小化按钮
    QLabel *welcome;
    QLabel *logo;
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QPushButton *okBtn;
    QPushButton *cancelBtn;
    QLabel *idLabel;

    QPoint m_pressedPoint; // for moving window
    bool   m_isPressed = false;

public slots:
    void sltCreate();
};


#endif // CREATEGROUPWND_H
