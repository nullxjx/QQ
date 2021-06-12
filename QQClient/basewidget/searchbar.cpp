#include "searchbar.h"
#include <QHBoxLayout>
#include <QPixmap>
#include <QDebug>
#include <QDateTime>

SearchBar::SearchBar(QWidget *parent, QSize p_size,int tag):
    QLineEdit(parent),tag(tag)
{   
    setFixedSize(p_size);
    IconHeight = p_size.height()/2;
    icon = new QLabel;
    icon->setPixmap(QPixmap(":/Icons/MainWindow/search.png").
                    scaled(IconHeight,IconHeight,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    icon->setFixedSize(QSize(IconHeight,IconHeight));
    icon->setStyleSheet("border:none;");

    QStringList list;
    list << ":/Icons/MainWindow/search_close.png"
         << ":/Icons/MainWindow/search_close.png"
         << ":/Icons/MainWindow/search_close.png";
    closeBtn = new MyButton(nullptr,list,QSize(IconHeight,IconHeight));
    closeBtn->setToolTip(tr("关闭"));
    closeBtn->setFocusPolicy(Qt::NoFocus);

    setPlaceholderText(tr("搜索..."));
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(icon);
    mainLayout->addStretch();
    mainLayout->addWidget(closeBtn);
    mainLayout->setContentsMargins(5,0,5,0);

    setTextMargins(IconHeight+5,0,IconHeight+5,0);
    setLayout(mainLayout);
    if(tag == 0)
        setStyleSheet("background-color:rgb(219, 217, 216);border:1px solid #eaeaea;border-radius:15px;");
    else if(tag == 1)
        setStyleSheet("background-color:rgb(219, 217, 216);border:2px solid #aeaeae;border-radius:20px;");

    setFont(QFont("Microsoft YaHei", 10, 50, false));
    connect(closeBtn,&MyButton::clicked,[&](){
        this->setText("");
        this->clearFocus();
    });

    closeBtn->setVisible(false);

    connect(this,&SearchBar::editingFinished,[&](){
        qDebug() << this->text();
    });
}

void SearchBar::focusInEvent(QFocusEvent *e)
{
    setStyleSheet("background-color:white;border:1px solid #eaeaea;border-radius:15px;");
    closeBtn->setVisible(true);
    this->setCursorPosition(0);
    return QLineEdit::focusInEvent(e);
}

void SearchBar::focusOutEvent(QFocusEvent *e)
{
    setStyleSheet("background-color:rgb(219, 217, 216);border:1px solid #eaeaea;border-radius:15px;");

    closeBtn->setVisible(false);

    if(tag == 0)
        setText("");

    return QLineEdit::focusOutEvent(e);
}


