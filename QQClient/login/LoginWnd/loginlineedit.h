/****************************************************************************
** QQ-Like 登录界面的用户名密码输入框
****************************************************************************/

#ifndef LOGINEDITLINE_H
#define LOGINEDITLINE_H

#include "lineeditex.h"
#include <QLabel>
#include <QLineEdit>
#include <QWidget>


class LoginLineEdit : public QWidget
{
    Q_OBJECT
public:
    explicit LoginLineEdit(QWidget *parent = nullptr);

public:
    void setLeftIcon(QString leftIconPathNormal, QString leftIconPathHover);
    void setBottomIcon(QString bottomLinePathNormal, QString bottomLinePathHover);
    void setLineEditStyleSheet(QString styleSheet);
    void setPlaceholderText(QString placeholderText);
    void setEchoMode(QLineEdit::EchoMode echoMode);

    QString text() const;
    void setText(QString text){m_lineEdit->setText(text);}
    void setFocus();

signals:
    void returnPressed();
    void textChanged();

public slots:
    void focusIn();
    void focusOut();

private:
    LineEditEx*  m_lineEdit;   // 编辑框
    QLabel*      m_leftIcon;   // 编辑框左边的图标
    QLabel*      m_rightIcon;  // 编辑框右边的图标
    QLabel*      m_bottomLine; // 编辑框下方的线条

    QString     m_leftIconPathNormal;
    QString     m_leftIconPathHover;
    QString     m_bottomLinePathNormal;
    QString     m_bottomLinePathHover;
};

#endif // LOGINEDITLINE_H
