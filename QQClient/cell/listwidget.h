#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QListWidget>
#include <QList>
#include <QHash>
#include <QMenu>
#include <QScrollBar>

#include "cell.h"
#include "cellviewdad.h"
#include "cellviewson.h"


class FloatingScrollBar;
class ListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit ListWidget(QWidget* parent = nullptr,int tag = 0);

    Cell *getRightClickedCell();//获取右击选中的格子
    void insertCell(Cell *cell);
    void removeCell(Cell *cell);
    void removeAllCells();
    void refreshList();
    Cell* getDadCellFromName(QString *name);
    void setDadPopMenu(QMenu *menu);
    void setSonPopMenu(QMenu *menu);
    void setCellToTop(Cell*);
    void cancelCellOnTop(Cell*);

    QList<Cell*> getAllCells() const;
    void resetCellState();
    void refreshCellTime(int id,qint64 time,QString msg);

signals:
    void popMenuToShow(Cell*,QMenu *);
    void sonDoubleClicked(Cell *son);
    void signalSonRightClicked();

    void signalOpenDialog(Cell* cell);

private:
    QList<Cell *> cells;//包含列表中所有的格子
    QList<CellViewSon*> sonItems;
    QMenu *cellDadMenu;
    QMenu *cellSonMenu;
    Cell *rightClickedCell;
    int tag = 0;
    FloatingScrollBar * scrollBar;

    void addSonItem(Cell *cell);
    void changeSonSelectionState(Cell*);

public slots:
    void onDadOpenChanged(CellViewDad *dad);
    void onSonSelected(Cell *son);
    void onCellRightClicked(Cell *cell);
    //void onPopMenuShow(QMenu *menu);
    void slt_valueChanged(int);

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void resizeEvent(QResizeEvent*);
};

class FloatingScrollBar : public QScrollBar
{
    Q_OBJECT

public:
    explicit FloatingScrollBar(QWidget *parent,Qt::Orientation t);
    ~FloatingScrollBar(){}

public slots:
    void slt_valueChanged(int);
    void slt_rangeChanged(int,int);
};

#endif // LISTWIDGET_H
