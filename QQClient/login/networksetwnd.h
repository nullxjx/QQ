#ifndef NETWORKSETWND_H
#define NETWORKSETWND_H

#include "windowbase.h"
#include "iplineedit.h"

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include "qiplineedit.h"


class NetWorkSetWnd : public WindowBase
{
    Q_OBJECT

public:
    explicit NetWorkSetWnd(QWidget *parent = nullptr);
    void readConfig();
    void showErrorMsg(QString);

signals:
    void rotateWindow();
    void closeWindow();
    void hideWindow();

public slots:
    void btnClicked();
    void hideErrorMsg();

private:
    QPushButton* m_menuCloseBtn;         // 菜单栏关闭按钮
    QPushButton* m_menuMinBtn;           // 菜单栏最小化按钮
    QLabel *titleLabel;
    QLabel *ipAddrLabel;
    QLabel *msgPortLabel;
    QLabel *filePortLabel;
    //IPLineEdit *ipEdit;
    QIPLineEdit *ipEdit;
    QLineEdit *msgPortEdit;
    QLineEdit *filePortEdit;
    QPushButton *cancelBtn;
    QPushButton *okBtn;

    QLabel *m_notifyMsg;
};

#endif // NETWORKSETWND_H
