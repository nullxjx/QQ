/****************************************************************************
** QQ-Like 头像显示 Label
****************************************************************************/

#ifndef HEADLABEL_H
#define HEADLABEL_H

#include <QLabel>
#include <QMouseEvent>

class HeadLabel : public QLabel
{
    Q_OBJECT
public:
    explicit HeadLabel(QWidget *parent = nullptr, QString headPhotoPath = "",
                       QString headPhotoShadowPath = "", QString headPhotoHighlightPath = "",int tag = 0);
    ~HeadLabel() override;
    void updateHead();

    void changeHead(QString head);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void enterEvent(QEvent*) override;
    virtual void leaveEvent(QEvent*) override;
    virtual void mousePressEvent(QMouseEvent *event) override;

signals:
    void clicked();

public slots:

private:
    QPixmap* m_headPhoto;           // 头像原图
    QPixmap* m_headPhotoRound;      // 显示的头像图
    QPixmap* m_headPhotoShadow;     // 边框阴影
    QPixmap* m_headPhotoHighlight;  // 高亮边框

    QSize    m_headPhotoSize;             // 显示头像的大小
    QSize    m_headPhotoHighlightSize;     // 显示头像高亮边框的大小

    QString headPhotoHighlightPath;

    int tag;
    bool mouseIn = false;
};

#endif // HEADLABEL_H
