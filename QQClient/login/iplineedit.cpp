#include "iplineedit.h"

#include <QLabel>
#include <QRegExpValidator>
#include <QPainter>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>

IPLineEdit::IPLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    QRegExp rx("(25[0-5]|2[0-4][0-9]|1?[0-9]{1,2})");
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(2);
    layout->setContentsMargins(2,2,2,2);
    setStyleSheet("background:transparent;");
    QLabel *labelDot[3];
    for(int i = 0;i < 4;i++){
        lineEdit[i] = new QLineEdit(this);
        lineEdit[i]->setProperty("ip",true);
        lineEdit[i]->setFrame(false);
        lineEdit[i]->setMaxLength(3);
        lineEdit[i]->setAlignment(Qt::AlignCenter);
        lineEdit[i]->installEventFilter(this);
        lineEdit[i]->setValidator(new QRegExpValidator(rx,this));
        lineEdit[i]->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
        lineEdit[i]->setStyleSheet("background:transparent;");

        layout->addWidget(lineEdit[i]);
        if (i < 3) {
            labelDot[i] = new QLabel(this);
            labelDot[i]->setText(".");
            labelDot[i]->setFont(QFont("Microsoft YaHei", 10, 100, false));
            labelDot[i]->setAlignment(Qt::AlignVCenter);
            labelDot[i]->setFixedWidth(3);
            layout->addWidget(labelDot[i]);
        }
    }

    this->setReadOnly(true);
    lineEdit[0]->setFocus();
    lineEdit[0]->selectAll();
}

IPLineEdit::~IPLineEdit()
{

}

void IPLineEdit::setText(const QString &strIP)
{
    //是否是IP地址
    if(!isTextValid(strIP)){
        QMessageBox::warning(this, "Attention",
                             "Your IP Address is Invalid!",
                             QMessageBox::StandardButton::Ok);
        return;
    }else{
        int i = 0;
        QStringList ipList = strIP.split(".");
        foreach (QString ip, ipList){
            lineEdit[i]->setText(ip);
            i++;
        }
    }
}

bool IPLineEdit::isTextValid(const QString &strIP)
{
    QRegExp rx2("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if (!rx2.exactMatch(strIP))
        return false;
    return true;
}

QString IPLineEdit::getText() const
{
    QString strIP;
    for(int i = 0;i < 4;i++){
        strIP.append(lineEdit[i]->text());
        if(i != 3)
            strIP.append(".");
    }
    return strIP;
}

bool IPLineEdit::eventFilter(QObject *obj, QEvent *e)
{
    if(children().contains(obj) && e->type() == QEvent::KeyPress){
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(e);
        QLineEdit *pCurrentEdit = qobject_cast<QLineEdit *>(obj);
        switch (keyEvent->key()){
        case Qt::Key_0:
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:
        {
            QString strText = pCurrentEdit->text();
            if (pCurrentEdit->selectedText().length()){
                pCurrentEdit->text().replace(pCurrentEdit->selectedText(), QChar(keyEvent->key()));
            } else if (strText.length() <=3 &&
                     strText.toInt() * 10 > 255){
                int index = getIndex(pCurrentEdit);
                if (index != -1 && index != 3){
                    lineEdit[index + 1]->setFocus();
                    lineEdit[index + 1]->selectAll();
                }
            } else if (strText.length() == 2 &&
                       strText.toInt() * 10 < 255){
                if (Qt::Key_0 == keyEvent->key() && strText.toInt()){
                    pCurrentEdit->setText(strText.insert(pCurrentEdit->cursorPosition(),
                                                         QChar(Qt::Key_0)));
                }
            }
            return QLineEdit::eventFilter(obj, e);
        }
        case Qt::Key_Backspace:
        {
            QString strText = pCurrentEdit->text();
            if (strText.isEmpty()){
                int index = getIndex(pCurrentEdit);
                if (index != -1 && index != 0){
                    lineEdit[index - 1]->setFocus();
                    int length = lineEdit[index - 1]->text().length();
                    lineEdit[index - 1]->setCursorPosition(length ? length : 0);
                }
            }
            return QLineEdit::eventFilter(obj, e);
        }
        case Qt::Key_Left:
        {
            if (!pCurrentEdit->cursorPosition()){
                int index = getIndex(pCurrentEdit);
                if (index != -1 && index != 0){
                    lineEdit[index - 1]->setFocus();
                    int length = lineEdit[index - 1]->text().length();
                    lineEdit[index - 1]->setCursorPosition(length ? length : 0);
                }
            }
            return QLineEdit::eventFilter(obj, e);
        }
        case Qt::Key_Right:
        {
            if (pCurrentEdit->cursorPosition() == pCurrentEdit->text().length()){
                int index = getIndex(pCurrentEdit);
                if (index != -1 && index != 3){
                    lineEdit[index + 1]->setFocus();
                    lineEdit[index + 1]->setCursorPosition(0);
                }
            }
            return QLineEdit::eventFilter(obj, e);
        }
            // 小键盘的“.”号
        case Qt::Key_Period:
        {
            int index = getIndex(pCurrentEdit);
            if (index != -1 && index != 3){
                lineEdit[index + 1]->setFocus();
                lineEdit[index + 1]->setCursorPosition(0);
            }
            return QLineEdit::eventFilter(obj, e);
        }
        default:
            break;
        }
    }
}

//获取当前输入框索引
int IPLineEdit::getIndex(QLineEdit *edit)
{
    int index = -1;
    for (int i = 0; i < 4; i++){
        if (edit == lineEdit[i])
            index = i;
    }
    return index;
}
