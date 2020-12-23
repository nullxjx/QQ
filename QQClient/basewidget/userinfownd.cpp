#include "userinfownd.h"
#include "database.h"
#include "windwmapi.h"
#include "myapp.h"

#include <QPainter>

#pragma comment(lib, "user32.lib")

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <windowsx.h>
#endif

UserInfoWnd::UserInfoWnd(QWidget *parent,int id,QString name,QString head,int sex)
    : QWidget(parent),id(id),name(name),head(head),sex(sex)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Popup);

    resize(370,300);

    QFont font = QFont("Microsoft YaHei", 17, 50, false);
    QFontMetrics fm(font);
    QFont font2 = QFont("Microsoft YaHei", 12, 50, false);

    nameLabel = new QLabel(name,this);
    nameLabel->setFont(font);
    idLabel = new QLabel("QQ号: " + QString::number(id),this);
    idLabel->setFont(font2);
    headLabel = new QLabel(this);
    headLabel->setPixmap(QPixmap(head).scaled(80,80,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    sexLabel = new QLabel(this);
    if(sex == 0){
        sexLabel->setPixmap(QPixmap(":/Icons/MainWindow/male.png").
                            scaled(20,20,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    }else if(sex == 1){
        sexLabel->setPixmap(QPixmap(":/Icons/MainWindow/female.png").
                            scaled(20,20,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    }

    bool tag = DataBase::Instance()->isMyFriend(id);
    if(tag){

        QJsonObject json = DataBase::Instance()->getFriendInfo(id);
        QString subgroup = json.value("subgroup").toString();
        subGroupLabel = new QLabel("分组: " + subgroup,this);
        subGroupLabel->setFont(font2);
        subGroupLabel->setGeometry(40,120,150,30);
    }

    //for test
    schoolLabel = new QLabel("学校: 江南大学",this);
    schoolLabel->setFont(font2);
    regionLabel = new QLabel("地区: 江苏 无锡",this);
    regionLabel->setFont(font2);
    //emailLabel = new QLabel("e-mail: thexjx@gmail.com",this);
    //emailLabel->setFont(font2);

    QStringList tmp;
    if(id == MyApp::m_nId){
        tmp << ":/Icons/MainWindow/editHead.png"
            << ":/Icons/MainWindow/editHead2.png"
            << ":/Icons/MainWindow/editHead2.png";
        sendMsgBtn = new MyButton(this,tmp,QSize(30,30));
        sendMsgBtn->setToolTip("编辑头像");
    }else{
        tmp << ":/Icons/MainWindow/sendMsg.png"
            << ":/Icons/MainWindow/sendMsg2.png"
            << ":/Icons/MainWindow/sendMsg2.png";
        sendMsgBtn = new MyButton(this,tmp,QSize(30,30));
        sendMsgBtn->setToolTip("发送消息");
    }

    int nameWidth = fm.width(nameLabel->text());
    int nameHeight = fm.height();
    //qDebug() << "nameWidth: " << nameWidth;

    if(nameWidth > 150)
        nameWidth = 150;
    nameLabel->setGeometry(40,40,nameWidth,nameHeight);
    QString  tmpStr = nameLabel->fontMetrics().elidedText(nameLabel->text(),Qt::ElideRight,
                                                          nameLabel->width(), Qt::TextShowMnemonic);
    //文字显示不下自动省略
    nameLabel->setText(tmpStr);

    sexLabel->setGeometry(40 + nameWidth + 10,50,20,20);
    idLabel->setGeometry(40,80,150,30);
    headLabel->setGeometry(250,40,80,80);
    sendMsgBtn->setGeometry(300,240,30,30);
    schoolLabel->setGeometry(40,170,150,30);
    regionLabel->setGeometry(40,210,150,30);
    //emailLabel->setGeometry(40,120,250,30);

    setFocusPolicy(Qt::StrongFocus);

    this->setStyleSheet("background-color:white;");

    //绘制窗口边缘阴影
    HWND hwnd = (HWND)this->winId();
    DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);

    // 此行代码可以带回Aero效果，同时也带回了标题栏和边框,在nativeEvent()会再次去掉标题栏
    ::SetWindowLong(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION);

    //we better left 1 piexl width of border untouch, so OS can draw nice shadow around it
    const MARGINS shadow = { 1, 1, 1, 1 };
    WinDwmapi::instance()->DwmExtendFrameIntoClientArea(HWND(winId()), &shadow);
}

void UserInfoWnd::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform,true);// 抗锯齿
    QPen pen;
    pen.setColor(QColor("#ebeae8"));
    painter.setPen(pen);
    painter.drawLine(40,this->height()/2,this->width()-40,this->height()/2);

    return QWidget::paintEvent(e);
}

void UserInfoWnd::focusInEvent(QFocusEvent *)
{
    //qDebug() << "userInfo wnd focus in";
}

void UserInfoWnd::focusOutEvent(QFocusEvent *)
{
    //qDebug() << "close userInfo wnd";
    this->close();
}

void UserInfoWnd::showEvent(QShowEvent *e)
{
    //qDebug() << "userInfo wnd show";
    this->setFocus();

    return QWidget::showEvent(e);
}

bool UserInfoWnd::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = (MSG *)message;
    switch (msg->message)
    {
    case WM_NCCALCSIZE:
    {
        // this kills the window frame and title bar we added with WS_THICKFRAME and WS_CAPTION
        *result = 0;
        return true;
    }
    default:
        return QWidget::nativeEvent(eventType, message, result);
    }
}
