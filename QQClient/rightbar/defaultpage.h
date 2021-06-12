#ifndef DEFAULTPAGE_H
#define DEFAULTPAGE_H

#include <QWidget>

class DefaultPage : public QWidget
{
    Q_OBJECT
public:
    explicit DefaultPage(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event);

signals:

public slots:
};

#endif // DEFAULTPAGE_H
