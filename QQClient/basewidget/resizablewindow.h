#ifndef RESIZABLEWINDOW_H
#define RESIZABLEWINDOW_H


#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QCursor>
#include <QRect>
#include <QSize>

#define PADDING 2

enum Direction{
    UP = 0,
    DOWN=1,
    LEFT,
    RIGHT,
    LEFTTOP,
    LEFTBOTTOM,
    RIGHTBOTTOM,
    RIGHTTOP,
    NONE
};

class ResizableWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ResizableWindow(QWidget *parent = nullptr);
    ~ResizableWindow();

    void region(const QPoint &cursorPoint);

signals:
    void changeWindowSize(QSize);

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private:
    bool isLeftPressDown;
    QPoint dragPosition;
    Direction dir;

};

#endif // RESIZABLEWINDOW_H
