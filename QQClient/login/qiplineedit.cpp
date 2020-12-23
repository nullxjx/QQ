#include "qiplineedit.h"

#include <QLabel>
#include <QRegExpValidator>
#include <QPainter>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>

QIPLineEdit::QIPLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    QRegExp rx("(25[0-5]|2[0-4][0-9]|1?[0-9]{1,2})");
    QHBoxLayout *pHBox = new QHBoxLayout(this);
    pHBox->setSpacing(2);
    pHBox->setContentsMargins(2, 2, 2, 2);
    QLabel *labelDot[3];
    for (int i = 0; i < 4; i++){
        m_lineEidt[i] = new QLineEdit(this);
        m_lineEidt[i]->setProperty("ip", true);
        m_lineEidt[i]->setFrame(false);
        m_lineEidt[i]->setMaxLength(3);
        m_lineEidt[i]->setAlignment(Qt::AlignCenter);
        m_lineEidt[i]->installEventFilter(this);
        m_lineEidt[i]->setValidator(new QRegExpValidator(rx, this));
        m_lineEidt[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        pHBox->addWidget(m_lineEidt[i]);
        if (i < 3) {
            labelDot[i] = new QLabel(this);
            labelDot[i]->setText(".");
            labelDot[i]->setFixedWidth(2);
            pHBox->addWidget(labelDot[i]);
        }

        connect(m_lineEidt[i],SIGNAL(textChanged(const QString &)),this,SIGNAL(textChanged()));
    }
    this->setReadOnly(true);
    m_lineEidt[0]->setFocus();
    m_lineEidt[0]->selectAll();

    setStyleSheet("background:transparent;");
}

QIPLineEdit::~QIPLineEdit()
{
}

// 获取当前输入框索引
int QIPLineEdit::getIndex(QLineEdit *pEdit)
{
    int index = -1;
    for (int i = 0; i < 4; i++){
        if (pEdit == m_lineEidt[i])
            index = i;
    }
    return index;
}

// 事件过滤器，判断按键输入
bool QIPLineEdit::eventFilter(QObject *obj, QEvent *ev)
{
    if (children().contains(obj) && QEvent::KeyPress == ev->type()){
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(ev);
        QLineEdit *pCurrentEdit = qobject_cast<QLineEdit *>(obj);
        switch (keyEvent->key())
        {
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
            if (pCurrentEdit->selectedText().length())
            {
                pCurrentEdit->text().replace(pCurrentEdit->selectedText(), QChar(keyEvent->key()));
            }
            else if (strText.length() <=3 &&
                     strText.toInt() * 10 > 255)
            {
                int index = getIndex(pCurrentEdit);
                if (index != -1 && index != 3)
                {
                    m_lineEidt[index + 1]->setFocus();
                    m_lineEidt[index + 1]->selectAll();
                }
            }
            else if (strText.length() == 2 && strText.toInt() * 10 < 255)
            {
                if (Qt::Key_0 == keyEvent->key() && strText.toInt())
                {
                    pCurrentEdit->setText(strText.insert(pCurrentEdit->cursorPosition(),
                                                         QChar(Qt::Key_0)));
                }
            }
            return QLineEdit::eventFilter(obj, ev);
        }
            break;
        case Qt::Key_Backspace:
        {
            QString strText = pCurrentEdit->text();
            if (strText.isEmpty())
            {
                int index = getIndex(pCurrentEdit);
                if (index != -1 && index != 0)
                {
                    m_lineEidt[index - 1]->setFocus();
                    int length = m_lineEidt[index - 1]->text().length();
                    m_lineEidt[index - 1]->setCursorPosition(length ? length : 0);
                }
            }
            return QLineEdit::eventFilter(obj, ev);
        }
        case Qt::Key_Left:
        {
            if (!pCurrentEdit->cursorPosition())
            {
                int index = getIndex(pCurrentEdit);
                if (index != -1 && index != 0)
                {
                    m_lineEidt[index - 1]->setFocus();
                    int length = m_lineEidt[index - 1]->text().length();
                    m_lineEidt[index - 1]->setCursorPosition(length ? length : 0);
                }
            }
            return QLineEdit::eventFilter(obj, ev);
        }
        case Qt::Key_Right:
        {
            if (pCurrentEdit->cursorPosition() == pCurrentEdit->text().length())
            {
                int index = getIndex(pCurrentEdit);
                if (index != -1 && index != 3)
                {
                    m_lineEidt[index + 1]->setFocus();
                    m_lineEidt[index + 1]->setCursorPosition(0);
                }
            }
            return QLineEdit::eventFilter(obj, ev);
        }
            // 小键盘的“.”号
        case Qt::Key_Period:
        {
            int index = getIndex(pCurrentEdit);
            if (index != -1 && index != 3)
            {
                m_lineEidt[index + 1]->setFocus();
                m_lineEidt[index + 1]->setCursorPosition(0);
            }
            return QLineEdit::eventFilter(obj, ev);
        }
            break;
        default:
            break;
        }
    }
    return false;
}

// 设置信息
void QIPLineEdit::setText(const QString &strIP)
{
    // 是否是IP地址
    if (!isTextValid(strIP))
    {
        QMessageBox::warning(this, "Attention",
                             "Your IP Address is Invalid!",
                             QMessageBox::StandardButton::Ok);
        return;
    }
    else
    {
        int i = 0;
        QStringList ipList = strIP.split(".");

        foreach (QString ip ,ipList)
        {
            m_lineEidt[i]->setText(ip);
            i++;
        }
    }
}

// 判断IP地址
bool QIPLineEdit::isTextValid(const QString &strIP)
{
    QRegExp rx2("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if (!rx2.exactMatch(strIP))
        return false;
    return true;
}


// 获取IP地址
QString QIPLineEdit::getText() const
{
    QString strIP;
    for (int i = 0; i < 4; i++) {
        strIP.append(m_lineEidt[i]->text());
        if (3 != i) {
            strIP.append(".");
        }
    }

    return strIP;
}

///////////////////////////////////////////////////////////////////
/// \brief QMacLineEdit::QMacLineEdit
/// \param parent
/// MAC地址输入框//////////////////////////////////////////////////
QMacLineEdit::QMacLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    QRegExp rx("([0-9A-Fa-f]{2})");
    QHBoxLayout *pHBox = new QHBoxLayout(this);
    pHBox->setSpacing(2);
    pHBox->setContentsMargins(2, 2, 2, 2);
    QLabel *labelDot[5];
    for (int i = 0; i < 6; i++)
    {
        m_lineEidt[i] = new QLineEdit(this);
        m_lineEidt[i]->setFrame(false);
        m_lineEidt[i]->setMaxLength(2);
        m_lineEidt[i]->setAlignment(Qt::AlignCenter);
        m_lineEidt[i]->installEventFilter(this);
        m_lineEidt[i]->setValidator(new QRegExpValidator(rx, this));
        m_lineEidt[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        pHBox->addWidget(m_lineEidt[i]);
        if (i < 5) {
            labelDot[i] = new QLabel(this);
            labelDot[i]->setText("-");
            labelDot[i]->setFixedWidth(2);
            pHBox->addWidget(labelDot[i]);
        }
    }
    this->setReadOnly(true);
    m_lineEidt[0]->setFocus();
    m_lineEidt[0]->selectAll();
}

QMacLineEdit::~QMacLineEdit()
{
}

int QMacLineEdit::getIndex(QLineEdit *pEdit)
{
    int index = -1;
    for (int i = 0; i < 6; i++)
    {
        if (pEdit == m_lineEidt[i])
            index = i;
    }
    return index;
}

//
bool QMacLineEdit::eventFilter(QObject *obj, QEvent *ev)
{
    if (children().contains(obj) && QEvent::KeyPress == ev->type())
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(ev);
        QLineEdit *pCurrentEdit = qobject_cast<QLineEdit *>(obj);

        switch (keyEvent->key())
        {
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
        case Qt::Key_A:
        case Qt::Key_B:
        case Qt::Key_C:
        case Qt::Key_D:
        case Qt::Key_E:
        case Qt::Key_F:
        {
            QString strText = pCurrentEdit->text();
            if (pCurrentEdit->selectedText().length())
            {
                pCurrentEdit->text().replace(pCurrentEdit->selectedText(),
                                             QChar(keyEvent->key()).toUpper());
            }
            else if (strText.length() == 2) {
                int index = getIndex(pCurrentEdit);
                if (0 <= index  && index < 5)
                {
                    m_lineEidt[index + 1]->setFocus();
                    m_lineEidt[index + 1]->selectAll();
                }
            }
            return QLineEdit::eventFilter(obj, ev);
        }
            break;
        case Qt::Key_Backspace:
        {
            QString strText = pCurrentEdit->text();
            if (strText.isEmpty())
            {
                int index = getIndex(pCurrentEdit);
                if (index != -1 && index != 0)
                {
                    m_lineEidt[index - 1]->setFocus();
                    int length = m_lineEidt[index - 1]->text().length();
                    m_lineEidt[index - 1]->setCursorPosition(length ? length : 0);
                }
            }
            return QLineEdit::eventFilter(obj, ev);
        }
        case Qt::Key_Period:
        {
            int index = getIndex(pCurrentEdit);
            QString strText = pCurrentEdit->text();
            if (strText.length() == 1) {
                pCurrentEdit->setText(strText.insert(0, QChar(Qt::Key_0)));
            }
            else if (strText.length() == 0) {
                pCurrentEdit->setText("00");
            }

            if (index != -1 && index < 5)
            {
                m_lineEidt[index + 1]->setFocus();
                m_lineEidt[index + 1]->setCursorPosition(0);
            }
            return QLineEdit::eventFilter(obj, ev);
        }
            break;
        default:
            break;
        }
    }
    return false;
}

//
void QMacLineEdit::setText(const QString &strMac)
{
    if (!isTextValid(strMac))
    {
        QMessageBox::warning(this, "Attention",
                             "Your MAC Address is Invalid!",
                             QMessageBox::StandardButton::Ok);
        return;
    }
    else
    {
        int i = 0;
        QStringList macList = strMac.split("-");

        foreach (QString mac ,macList)
        {
            m_lineEidt[i]->setText(mac);
            i++;
        }
    }
}

bool QMacLineEdit::isTextValid(const QString &strIP)
{
    QRegExp rx2("([0-9A-Za-z]{2})([0-9A-Za-z:-]{3}){5}");
    if (!rx2.exactMatch(strIP))
        return false;

    return true;
}

QString QMacLineEdit::text() const
{
    QString strMac;
    for (int i = 0; i < 5; i++) {
        strMac.append(m_lineEidt[i]->text());
        if (3 != i) {
            strMac.append("-");
        }
    }
    return strMac;
}

///////////////////////////////////////////////////////////
/// \brief QIconLineEdit::QIconLineEdit
/// \param parent
/// 带图片的linedit
QIconLineEdit::QIconLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    labelPixmap = new QLabel(this);
    labelPixmap->setMinimumSize(16, 16);
    labelPixmap->setVisible(false);

}

QIconLineEdit::~QIconLineEdit()
{

}

void QIconLineEdit::SetIcon(const QPixmap &pixmap)
{
    if (pixmap.isNull()) return;

    labelPixmap->setPixmap(pixmap);
    labelPixmap->setVisible(true);
    labelPixmap->setGeometry(5,(this->height() - pixmap.height()) / 2, 16, 16);
    this->setTextMargins(25, 1, 1, 1);
}

