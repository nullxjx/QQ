#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include "mybutton.h"

#include <QLineEdit>
#include <QLabel>

class SearchBar : public QLineEdit
{
    Q_OBJECT

public:
    SearchBar(QWidget *parent = nullptr, QSize p_Size = QSize(0,0),int tag = 0);

private:
    QLabel *icon;
    MyButton *closeBtn;
    int IconHeight;
    int tag;

protected:
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
};

#endif // SEARCHBAR_H
