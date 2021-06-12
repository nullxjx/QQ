#include "mytextedit.h"
#include "myapp.h"

#include <QDebug>

MyTextEdit::MyTextEdit(QWidget *parent):
    QTextEdit(parent)
{

}


void MyTextEdit::mousePressEvent(QMouseEvent *e)
{
    emit mousePressed();
    return QTextEdit::mousePressEvent(e);
}

void MyTextEdit::focusOutEvent(QFocusEvent *e)
{
    emit focusOut();
    return QTextEdit::focusOutEvent(e);
}

void MyTextEdit::keyPressEvent(QKeyEvent *e)
{
    if(MyApp::sendWay){//按enter发送
        if(e->key() == Qt::Key_Return){
            emit sendMsg();
        }else{
            return QTextEdit::keyPressEvent(e);
        }
    }else{
        if(e->key() == Qt::Key_Return && e->modifiers() == Qt::ControlModifier)
            emit sendMsg();
        return QTextEdit::keyPressEvent(e);
    }
}

