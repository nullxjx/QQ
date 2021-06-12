#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QTextEdit>
#include <QWidget>
#include <QKeyEvent>

class MyTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    MyTextEdit(QWidget *parent = nullptr);

signals:
    void mousePressed();
    void focusOut();
    void sendMsg();

protected:

    void mousePressEvent(QMouseEvent*);
    void focusOutEvent(QFocusEvent *);
    void keyPressEvent(QKeyEvent*);
};

#endif // MYTEXTEDIT_H
