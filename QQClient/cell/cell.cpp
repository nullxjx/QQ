#include "cell.h"

#include <QDebug>

Cell::Cell()
{
    isOpen = false;
    isClicked = false;
}

void Cell::SetSubtitle(const QString &text)
{
    if (text.isEmpty()) return;
    subTitle = text;
}

void Cell::SetIconPath(const QString &path)
{
    if (path.isEmpty()) return;
    iconPath = path;
}

void Cell::SetStatus(const quint8 &val)
{
    status = val;
}

void Cell::sortById()
{
    std::sort(childs.begin(),childs.end(),[](Cell* cellA, Cell* cellB){
        return cellA->id < cellB->id;
    });

    qDebug() << "done";
}

void Cell::sortByName()
{
    std::sort(childs.begin(),childs.end(),[](Cell* cellA, Cell* cellB){
        return cellA->name < cellB->name;
    });
}
