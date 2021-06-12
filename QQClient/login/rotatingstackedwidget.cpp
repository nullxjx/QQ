#include "rotatingstackedwidget.h"
#include "global.h"

#include <QVariant>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QTransform>
#include <QDebug>
#include <QGraphicsDropShadowEffect>

RotatingStackedWidget::RotatingStackedWidget(QWidget *parent) :
    QStackedWidget(parent)
{
    m_isRoratingWindow = false;
    m_nextPageIndex = 0;
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    // 给窗口设置rotateValue属性;
    this->setProperty("rotateValue", 0);
    initRotateWindow();
}

RotatingStackedWidget::~RotatingStackedWidget()
{

}

// 初始化旋转的窗口;
void RotatingStackedWidget::initRotateWindow()
{
    loginWnd = new LoginWnd(this);
    // 这里定义了两个信号，需要自己去发送信号;
    connect(loginWnd, SIGNAL(rotateWindow()), this, SLOT(onRotateWindow()));
    connect(loginWnd, SIGNAL(closeWindow()), this, SLOT(close()));
    connect(loginWnd, SIGNAL(hideWindow()), this, SLOT(onHideWindow()));
    connect(loginWnd, SIGNAL(openRegisterWnd()), this, SLOT(sltOpenRegisterWnd()));
    connect(loginWnd, SIGNAL(openChangePasswordWnd()),
            this, SLOT(sltOpenChangePasswordWnd()));


    netWorkSetWnd = new NetWorkSetWnd(this);
    connect(netWorkSetWnd, SIGNAL(rotateWindow()), this, SLOT(onRotateWindow()));
    connect(netWorkSetWnd, SIGNAL(closeWindow()), this, SLOT(close()));
    connect(netWorkSetWnd, SIGNAL(hideWindow()), this, SLOT(onHideWindow()));

    this->addWidget(loginWnd);
    this->addWidget(netWorkSetWnd);

    // 这里宽和高都增加，是因为在旋转过程中窗口宽和高都会变化;
    this->setContentsMargins(20,100,20,100);
    this->setFixedSize(QSize(loginWnd->width() + 40, loginWnd->height() + 200));

    QTimer::singleShot(1500,this,SLOT(sltAutoLogin()));

    registerWnd = new RegisterWnd();
    registerWnd->hide();
    connect(registerWnd,SIGNAL(closeWindow(QPoint)),
            this,SLOT(sltCloseRegisterWnd(QPoint)));
    connect(registerWnd,SIGNAL(signalRegister(const QJsonValue &)),
            loginWnd,SLOT(sltRegister(const QJsonValue &)));
    connect(loginWnd,SIGNAL(signalRegisterOK(const QJsonValue&)),
            registerWnd,SLOT(sltRegisterOK(const QJsonValue&)));

    changePasswordWnd = new ChangePasswordWnd();
    connect(changePasswordWnd,SIGNAL(closeWindow(QPoint)),
            this,SLOT(sltCloseFindwordWnd(QPoint)));
    connect(changePasswordWnd,SIGNAL(signalChangePwd(const QJsonValue &)),
            loginWnd,SLOT(sltChangePwd(const QJsonValue &)));
    connect(loginWnd,SIGNAL(signalChangePwdReply(const QJsonValue &)),
            changePasswordWnd,SLOT(sltChangePwdReply(const QJsonValue &)));

    changePasswordWnd->hide();
}

// 开始旋转窗口;
void RotatingStackedWidget::onRotateWindow()
{
    // 如果窗口正在旋转，直接返回;
    if (m_isRoratingWindow)
        return;

    qDebug() << "start rotating...";

    m_isRoratingWindow = true;
    m_nextPageIndex = (currentIndex() + 1) >= count() ? 0 : (currentIndex() + 1);
    QPropertyAnimation *rotateAnimation = new QPropertyAnimation(this, "rotateValue");
    // 设置旋转持续时间;
    rotateAnimation->setDuration(1000);
    // 设置旋转角度变化趋势;
    rotateAnimation->setEasingCurve(QEasingCurve::InCubic);
    // 设置旋转角度范围;
    rotateAnimation->setStartValue(0);
    rotateAnimation->setEndValue(180);
    connect(rotateAnimation, SIGNAL(valueChanged(QVariant)), this, SLOT(repaint()));
    connect(rotateAnimation, SIGNAL(finished()), this, SLOT(onRotateFinished()));
    // 隐藏当前窗口，通过不同角度的绘制来达到旋转的效果;
    currentWidget()->hide();
    rotateAnimation->start();
}

// 旋转结束;
void RotatingStackedWidget::onRotateFinished()
{
    m_isRoratingWindow = false;
    setCurrentWidget(widget(m_nextPageIndex));
    repaint();
}

// 绘制旋转效果;
void RotatingStackedWidget::paintEvent(QPaintEvent *event)
{
    if (m_isRoratingWindow){
        loginWnd->setGraphicsEffect(nullptr);
        netWorkSetWnd->setGraphicsEffect(nullptr);

        int rotateValue = this->property("rotateValue").toInt();
        if (rotateValue <= 90)// 小于90度时;
        {
            QPixmap rotatePixmap(currentWidget()->size());
            currentWidget()->render(&rotatePixmap);
            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing);
            QTransform transform;
            transform.translate(width() / 2, 0);
            transform.rotate(rotateValue, Qt::YAxis);
            painter.setTransform(transform);
            painter.drawPixmap(-1 * width() / 2 + 20, 100, rotatePixmap);
        } else {
            // 大于90度时
            QPixmap rotatePixmap(widget(m_nextPageIndex)->size());
            widget(m_nextPageIndex)->render(&rotatePixmap);
            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing);
            QTransform transform;
            transform.translate(width() / 2, 0);
            transform.rotate(rotateValue + 180, Qt::YAxis);
            painter.setTransform(transform);
            painter.drawPixmap(-1 * width() / 2 + 20, 100, rotatePixmap);
        }
    } else {
        if(m_nextPageIndex == 0){
            QGraphicsDropShadowEffect *pEffect = new QGraphicsDropShadowEffect(loginWnd);
            pEffect->setOffset(0, 0);
            pEffect->setColor(QColor(QStringLiteral("black")));
            pEffect->setBlurRadius(30);
            loginWnd->setGraphicsEffect(pEffect);
        }else {
            QGraphicsDropShadowEffect *pEffect = new QGraphicsDropShadowEffect(netWorkSetWnd);
            pEffect->setOffset(0, 0);
            pEffect->setColor(QColor(QStringLiteral("black")));
            pEffect->setBlurRadius(30);
            netWorkSetWnd->setGraphicsEffect(pEffect);
        }

        return QWidget::paintEvent(event);
    }
}

void RotatingStackedWidget::onHideWindow()
{
    showMinimized();
}

void RotatingStackedWidget::sltAutoLogin()
{
    loginWnd->checkAutoLogin();
}

void RotatingStackedWidget::sltOpenRegisterWnd()
{
    registerWnd->move(this->pos().x(),this->pos().y() + 80);
    registerWnd->show();
    this->hide();
}

void RotatingStackedWidget::sltCloseRegisterWnd(QPoint pos)
{
    this->move(pos.x(),pos.y() - 80);
    this->show();
}

void RotatingStackedWidget::sltOpenChangePasswordWnd()
{
    changePasswordWnd->move(this->pos().x(),this->pos().y() + 80);
    changePasswordWnd->show();
    this->hide();
}

void RotatingStackedWidget::sltCloseFindwordWnd(QPoint pos)
{
    this->move(pos.x(),pos.y() - 80);
    this->show();
}

void RotatingStackedWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_pressedPoint = event->globalPos() - this->pos();
        m_isPressed = true;
        event->accept();
    }
}

void RotatingStackedWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressed = false;
    event->accept();
}

void RotatingStackedWidget::mouseMoveEvent(QMouseEvent *event)
{
    if((event->buttons()&(Qt::LeftButton)) && m_isPressed == true){
        move(event->globalPos() - m_pressedPoint);
        event->accept();
    }
}

