/****************************************************************************
** QQ-Like 实现窗口常用的功能
****************************************************************************/

#ifndef WINDOWBASE_H
#define WINDOWBASE_H

#include <QMainWindow>
#include <QTimer>

class WindowBase : public QWidget
{
    Q_OBJECT
public:
    explicit WindowBase(QWidget *parent = nullptr);

protected:
    // 绘制窗口的圆角和边框阴影
    void paintEventForRoundAndBorder(int roundRadius = 0, int borderWidth = 1);
    virtual void paintEvent(QPaintEvent *) override;

#if 0
    // 重写鼠标操作以实现移动窗口
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

    // ESC 按下后关闭窗口
    virtual void keyPressEvent(QKeyEvent *event) override;

#endif

signals:

public slots:
    void changeBackground();

private:
    QPoint m_pressedPoint; // for moving window
    bool   m_isPressed = false;

    qreal    m_bkgOpacity;         // 下一张背景图的不透明度
    qreal    m_bkgOpacityInterval; // 每次改变背景图的不透明度
    qint8    m_currentBkgIdx;      // 当前背景图的索引
    bool     m_increasing;         // 当前背景图的值正在递增
    QTimer*  m_bkgTimer;           // 背景图动画的计时器

    qint8 getNextBkgIdx();
    void  paintBackground(qint8 bkgIdx, qreal bkgOpacity);

    const static QString LW_BACKGOUND_PREFIX;   // 背景图的前缀
    const static quint8  LW_BACKGOUND_NUM;      // 背景图数量
};


#endif // WINDOWBASE_H
