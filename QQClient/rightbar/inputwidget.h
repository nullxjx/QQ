#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include "mybutton.h"
#include "mytextedit.h"

#include <QWidget>
#include <QPushButton>

class InputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InputWidget(QWidget *parent = nullptr);

    MyButton *picture;
    MyButton *file;
    MyButton *screenshot;
    MyButton *emoji;
    QPushButton *sendBtn;
    QPushButton *actionBtn;
    MyTextEdit *textEdit;

    void paintEvent(QPaintEvent *);

signals:

public slots:
    void changeColor();
    void restoreColor();
    void saveSendWay(QAction *);
};

#endif // INPUTWIDGET_H
