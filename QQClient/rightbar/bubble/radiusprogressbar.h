#ifndef RADIUSPROGRESSBAR_H
#define RADIUSPROGRESSBAR_H

#include <QProgressBar>

class RadiusProgressBar : public QProgressBar
{
    Q_OBJECT
public:
    explicit RadiusProgressBar(QWidget *parent = nullptr);
    void setTag(int tag){this->tag = tag;}

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    int tag;
};

#endif // RADIUSPROGRESSBAR_H
