#include "bubblelist.h"

#include <QDebug>
#include <QScrollBar>
#include <QStylePainter>
#include <QDateTime>
#include <QFileInfo>
#include <QProcess>
#include <QApplication>
#include <QClipboard>

BubbleList::BubbleList(QWidget *parent) :
    QListWidget(parent)
{
    this->setVerticalScrollMode(QListWidget::ScrollPerPixel);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("border:none;background-color:#F0F0F0;"
                  "BubbleList::item:hover{background:#F0F0F0;border:none;}"
                  "BubbleList::item:selected{background:#F0F0F0;border:none;}");
    setContentsMargins(0,0,0,0);

    scrollBar = new FloatingScrollBar(this,Qt::Vertical);
    connect(this->verticalScrollBar(),SIGNAL(valueChanged(int)),
            scrollBar,SLOT(slt_valueChanged(int)));
    connect(scrollBar,SIGNAL(valueChanged(int)),this,SLOT(slt_valueChanged(int)));
    connect(this->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),
            scrollBar,SLOT(slt_rangeChanged(int,int)));

    oldWidth = this->width();
}

void BubbleList::insertBubble(BubbleInfo *b)
{
    bubbles.append(b);
    addBubbleItem(b);
    setCurrentRow(count() - 1);

    //refreshList();
}

void BubbleList::addBubbleItem(BubbleInfo * info)
{
    BubbleView * view = new BubbleView(nullptr,info);
    //connect(info,SIGNAL(hideAnimation()),view,SLOT(sltHideAnimation()));
    view->resize(this->width(),0);

    //设置右击菜单
    QMenu *PopMenu = new QMenu(this);
    if(info->msgType == Text){
        PopMenu->addAction(tr("复制"));
        PopMenu->addAction(tr("清屏"));
    } else if(info->msgType == Files){
        if(info->sender == You){
            if(info->downloaded){
                PopMenu->addAction(tr("打开文件所在文件夹"));
            }else{
                PopMenu->addAction(tr("下载"));
            }
            PopMenu->addAction(tr("清屏"));
        }else if(info->sender == Me){
            PopMenu->addAction(tr("打开文件所在文件夹"));
            PopMenu->addAction(tr("清屏"));
        }
    } else if(info->msgType == Picture){
        PopMenu->addAction(tr("打开图片所在文件夹"));
        PopMenu->addAction(tr("清屏"));
    }

    view->setPopMenu(PopMenu);
    if(info->sender != System){
        connect(view,SIGNAL(mouseRightClicked(BubbleInfo*,QMenu*)),
                this,SLOT(sltshowPopMenu(BubbleInfo*,QMenu*)));
        connect(PopMenu,SIGNAL(triggered(QAction*)),this,SLOT(onSonMenuSelected(QAction*)));
    }

    QListWidgetItem *item = new QListWidgetItem("");

    QSize size = view->setRect();
    view->setFixedSize(size);
    item->setSizeHint(size);
    //qDebug() << "Bubble size: " << size;

    this->addItem(item);
    this->setItemWidget(item,view);
}

void BubbleList::refreshList()
{
    this->clear();
    for(BubbleInfo *info : bubbles){
        addBubbleItem(info);
    }
    setCurrentRow(count() - 1);
}

void BubbleList::wapeOut()
{
    //清屏
    bubbles.clear();
    refreshList();
}

void BubbleList::msgConfirmed(qint64 time)
{
    for(BubbleInfo* b: bubbles){
        if(b->time == time){
            qDebug() << "cur time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd  hh:mm:ss.zzz");
            qDebug() << "[msg:" << b->msg << "  send time:"
                     << QDateTime::fromSecsSinceEpoch(time).toString("yyyy-MM-dd  hh:mm:ss.zzz")
                     << "] is received by the server" << endl;

            b->showAnimation = false;
            //emit b->hideAnimation();

            refreshList();
            return;
        }
    }
}

BubbleInfo *BubbleList::findBubble(int senderID, qint64 time)
{
    int cnt = bubbles.size();
    for(int i = 0;i < cnt;i++){
        BubbleInfo *bubble = bubbles.at(i);
        if(bubble->yourID == senderID && bubble->time == time){
            return bubble;
        }
    }
    return nullptr;
}

void BubbleList::resizeEvent(QResizeEvent *e)
{
    //qDebug() << "BubbleList size:" << this->size();
    if(oldWidth != this->width()){
        oldWidth = this->width();
        refreshList();
    }

    int x = this->width() - 8;
    scrollBar->setGeometry(x,1,8,this->height()-2);

    return QListWidget::resizeEvent(e);
}

void BubbleList::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(viewport());
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void BubbleList::enterEvent(QEvent *e)
{
    //this->verticalScrollBar()->setVisible(true);
    if(scrollBar->maximum() > 0){
        scrollBar->show();
    }

    return QListWidget::enterEvent(e);
}

void BubbleList::leaveEvent(QEvent *e)
{
    //this->verticalScrollBar()->setVisible(false);
    scrollBar->hide();
    return QListWidget::leaveEvent(e);
}

void BubbleList::slt_valueChanged(int value)
{
    this->verticalScrollBar()->setValue(value);
}

void BubbleList::onSonMenuSelected(QAction *action)
{
    if(!action->text().compare(tr("复制"))){
        qDebug() << "复制文本";

        QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
        //QString originalText = clipboard->text();          //获取剪贴板上文本信息
        clipboard->setText(curBubble->msg);
    }else if(!action->text().compare(tr("清屏"))){
        qDebug() << "清屏";
        this->wapeOut();

    }else if(!action->text().compare(tr("下载"))){
        qDebug() << "下载";
        QJsonObject json;
        json.insert("from",curBubble->yourID);
        json.insert("to",curBubble->myID);
        json.insert("group",curBubble->groupID);
        json.insert("time",curBubble->time);
        json.insert("tag",curBubble->tag);

        emit signalSendMessage(GetFile,json);

    }else if(!action->text().compare(tr("打开图片所在文件夹")) ||
             !action->text().compare(tr("打开文件所在文件夹"))){
        QString filepath = curBubble->msg;

        /*
        QFileInfo fileInfo(filepath);
        qDebug() << "path:" << fileInfo.path();
        QString path = fileInfo.path();//获取程序当前目录
        path.replace("/","\\");//将地址中的"/"替换为"\"，因为在Windows下使用的是"\"。
        QProcess::startDetached("explorer "+path);//打开上面获取的目录
        */

        QString path = filepath;//获取程序当前目录
        path.replace("/","\\");//将地址中的"/"替换为"\"，因为在Windows下使用的是"\"。
        //qDebug() << path;
        QString cmd = QString("explorer.exe /select," + path);
        //qDebug() << cmd;
        QProcess::startDetached(cmd);
    }
}

void BubbleList::sltshowPopMenu(BubbleInfo *info, QMenu *)
{
    curBubble = info;
}
