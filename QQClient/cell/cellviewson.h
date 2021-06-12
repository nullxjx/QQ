#ifndef CELLVIEWSON_H
#define CELLVIEWSON_H

#include "cell.h"
#include "mybutton.h"
#include "roundlabel.h"

#include <QWidget>
#include <QLabel>
#include <QMenu>

class CellViewSon : public QWidget
{
    Q_OBJECT
public:
    explicit CellViewSon(QWidget *parent = nullptr,Cell *c = nullptr,int tag = 0);
    void setPopMenu(QMenu *menu);

signals:
    void onSelected(Cell *cell);
    void onDoubleClicked(Cell *cell);
    void onRightClicked(Cell *cell);
    void onPopMenuToShow(Cell*,QMenu*);

    void signalOpenDialog(Cell* cell);

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void paintEvent(QPaintEvent *);
    void focusOutEvent(QFocusEvent*);

public :
    Cell *cell;
    QMenu *popMenu;

private:
    bool entered;   //记录光标是否移入
    int tag = 0;
    bool isAdmin = false;

    //QLabel *headIcon;
    QLabel *newMsg;
    RoundLabel *headLabel;
    //QLabel *headLabel;
    QLabel *notice;
    QLabel *adminLabel;
    QLabel *topLabel;
};

#endif // CELLVIEWSON_H
