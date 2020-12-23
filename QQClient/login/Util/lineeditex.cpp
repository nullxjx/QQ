/****************************************************************************
** QQ-Like 对 Qt 的 QLineEdit 进行扩展
****************************************************************************/

#include "lineeditex.h"

#include <QDateTime>
#include <QDebug>

LineEditEx::LineEditEx(QWidget *parent) : QLineEdit(parent)
{

}

void LineEditEx::focusInEvent(QFocusEvent *event)
{
    emit focusInSignal();
    return QLineEdit::focusInEvent(event);
}

void LineEditEx::focusOutEvent(QFocusEvent *event)
{
    emit focusOutSignal();
    return QLineEdit::focusOutEvent(event);
}
