/****************************************************************************
** QQ-Like 自定义的 CheckBox
****************************************************************************/
#include "checkbox.h"

#include <QMouseEvent>

const QString CheckBox::CHECKBOX_NORMAL_ICON_PATHS[2] = {":/loginwnd/checkbox_normal", ":/loginwnd/checkbox_tick_normal"};
const QString CheckBox::CHECKBOX_HOVER_ICON_PATHS[2] = {":/loginwnd/checkbox_hover", ":/loginwnd/checkbox_tick_highlight"};
const QString CheckBox::CHECKBOX_PRESS_ICON_PATHS[2] = {":/loginwnd/checkbox_press", ":/loginwnd/checkbox_tick_pushed"};

CheckBox::CheckBox(bool checked, QWidget *parent) : QWidget(parent), m_isChecked(checked),
    m_leftIcon(nullptr), m_textLabel(nullptr)
{
    m_leftIcon = new QLabel(this);
    m_leftIcon->setPixmap(QPixmap(CHECKBOX_NORMAL_ICON_PATHS[m_isChecked]));
    m_leftIcon->setFixedSize(14, 14);
    m_leftIcon->move(3, 3);

    m_textLabel = new QLabel(this);
    m_textLabel->setStyleSheet("font-size: 12px;font-family:Microsoft YaHei; color: #a6a6a6");
    m_textLabel->setFixedSize(55, 20);
    m_textLabel->move(20, 0);
}

void CheckBox::setText(QString text)
{
    m_textLabel->setText(text);
}

void CheckBox::mousePressEvent(QMouseEvent *event)
{
    m_isChecked = !m_isChecked;
    emit statusChanged(m_isChecked);

    event->accept();
    m_leftIcon->setPixmap(QPixmap(CHECKBOX_PRESS_ICON_PATHS[m_isChecked]));
}

void CheckBox::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    m_leftIcon->setPixmap(QPixmap(CHECKBOX_NORMAL_ICON_PATHS[m_isChecked]));
}

void CheckBox::enterEvent(QEvent *)
{
    m_leftIcon->setPixmap(QPixmap(CHECKBOX_HOVER_ICON_PATHS[m_isChecked]));
}

void CheckBox::leaveEvent(QEvent *)
{
    m_leftIcon->setPixmap(QPixmap(CHECKBOX_NORMAL_ICON_PATHS[m_isChecked]));
}

void CheckBox::changeStatus(bool isChecked)
{
    m_isChecked = isChecked;
    m_leftIcon->setPixmap(QPixmap(CHECKBOX_NORMAL_ICON_PATHS[m_isChecked]));
}
