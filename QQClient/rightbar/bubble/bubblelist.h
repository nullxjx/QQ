#ifndef CHATBUBBLE_H
#define CHATBUBBLE_H

#include "bubbleview.h"
#include "listwidget.h"

#include <QListWidget>
#include <QList>
#include <QResizeEvent>

class BubbleList : public QListWidget
{
    Q_OBJECT
public:
    explicit BubbleList(QWidget *parent = nullptr);

    void insertBubble(BubbleInfo*);
    void addBubbleItem(BubbleInfo*);
    void refreshList();
    void wapeOut();

    void msgConfirmed(qint64 time);

    BubbleInfo* findBubble(int senderID,qint64 time);

private:
    QList<BubbleInfo*> bubbles;
    int oldWidth;

    FloatingScrollBar *scrollBar;

    BubbleInfo *curBubble;

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent*);

signals:
    void popMenuToShow(BubbleInfo*,QMenu*);
    void signalSendMessage(const quint8 &, const QJsonValue &);

public slots:
    void slt_valueChanged(int);
    void onSonMenuSelected(QAction*);
    void sltshowPopMenu(BubbleInfo*,QMenu*);
};

#endif // CHATBUBBLE_H
