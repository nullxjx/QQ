/****************************************************************************
** QQ-Like 自定义的 CheckBox
****************************************************************************/
#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <QLabel>
#include <QWidget>


class CheckBox : public QWidget
{
    Q_OBJECT
public:
    explicit CheckBox(bool checked = false, QWidget *parent = nullptr);
    void setText(QString text);

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void enterEvent(QEvent *) override;
    virtual void leaveEvent(QEvent *) override;

signals:
    void statusChanged(bool isChecked);

public slots:
    void changeStatus(bool isChecked);

private:
    bool     m_isChecked;
    QLabel*  m_leftIcon;    // CheckBox 左边的图标
    QLabel*  m_textLabel;   // CheckBox 右边的文字

    const static QString CHECKBOX_NORMAL_ICON_PATHS[2];
    const static QString CHECKBOX_HOVER_ICON_PATHS[2];
    const static QString CHECKBOX_PRESS_ICON_PATHS[2];
};


#endif // CHECKBOX_H
