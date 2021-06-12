/****************************************************************************
** QQ-Like 头像显示 Label
****************************************************************************/
#include "headlabel.h"
#include "myapp.h"

#include <QBitmap>
#include <QPainter>


HeadLabel::HeadLabel(QWidget *parent, QString headPhotoPath, QString headPhotoShadowPath, QString headPhotoHighlightPath,int tag)
    : QLabel(parent), m_headPhoto(nullptr), m_headPhotoRound(nullptr),m_headPhotoShadow(nullptr), m_headPhotoHighlight(nullptr),tag(tag)
{
    if(!headPhotoPath.isEmpty())
        m_headPhoto = new QPixmap(headPhotoPath);
    if(!headPhotoShadowPath.isEmpty())
        m_headPhotoShadow = new QPixmap(headPhotoShadowPath);
    if(!headPhotoHighlightPath.isEmpty())
        m_headPhotoHighlight = new QPixmap(headPhotoHighlightPath);

    this->headPhotoHighlightPath = headPhotoHighlightPath;
}


HeadLabel::~HeadLabel()
{
    if(m_headPhoto != nullptr)
        delete  m_headPhoto;
    if(m_headPhotoRound != nullptr)
        delete  m_headPhotoRound;
    if(m_headPhotoShadow != nullptr)
        delete  m_headPhotoShadow;
    if(m_headPhotoHighlight != nullptr)
        delete  m_headPhotoHighlight;
}

void HeadLabel::updateHead()
{
    m_headPhoto = new QPixmap(MyApp::m_strHeadPath + MyApp::m_strHeadFile);
}

void HeadLabel::changeHead(QString head)
{
    m_headPhoto = new QPixmap(head);

    if(m_headPhoto != nullptr){
        if(m_headPhotoRound != nullptr)
            delete m_headPhotoRound;

        m_headPhotoSize = {this->size().width()*31/49, this->size().height()*31/49};
        QBitmap mask(m_headPhotoSize);
        QPainter maskPainter(&mask);
        maskPainter.setRenderHint(QPainter::Antialiasing);
        maskPainter.setRenderHint(QPainter::SmoothPixmapTransform);
        maskPainter.fillRect(0, 0, m_headPhotoSize.width(), m_headPhotoSize.height(), Qt::white);
        maskPainter.setBrush(QColor(0, 0, 0));
        maskPainter.drawRoundedRect(0, 0, m_headPhotoSize.width(), m_headPhotoSize.height(), 100, 100);
        m_headPhotoRound = new QPixmap(m_headPhoto->scaled(m_headPhotoSize));
        m_headPhotoRound->setMask(mask);
    }


    update();
}


void HeadLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);

    if(m_headPhotoRound != nullptr) {
        int tx = (this->width()-m_headPhotoSize.width())/2;
        int ty = (this->height()-m_headPhotoSize.height())/2;
        painter.drawPixmap(tx, ty, *m_headPhotoRound);
    }

    if(m_headPhotoHighlight != nullptr){
        int tx = (this->width()-m_headPhotoHighlightSize.width())/2;
        int ty = (this->height()-m_headPhotoHighlightSize.height())/2;
        painter.drawPixmap(tx, ty, m_headPhotoHighlightSize.width(),
                           m_headPhotoHighlightSize.height(), *m_headPhotoHighlight);
    }

    if(tag == 0){
        if(m_headPhotoShadow != nullptr)
            painter.drawPixmap(0, 0, *m_headPhotoShadow);
    }else if(tag == 1){
        if(mouseIn){
            if(m_headPhotoShadow != nullptr)
                painter.drawPixmap(0, 0, *m_headPhotoShadow);
        }
    }

    return QLabel::paintEvent(event);
}


void HeadLabel::resizeEvent(QResizeEvent *event)
{
    // 创建圆形头像
    if(m_headPhoto != nullptr){
        if(m_headPhotoRound != nullptr)
            delete m_headPhotoRound;

        m_headPhotoSize = {this->size().width()*31/49, this->size().height()*31/49};
        QBitmap mask(m_headPhotoSize);
        QPainter maskPainter(&mask);
        maskPainter.setRenderHint(QPainter::Antialiasing);
        maskPainter.setRenderHint(QPainter::SmoothPixmapTransform);
        maskPainter.fillRect(0, 0, m_headPhotoSize.width(), m_headPhotoSize.height(), Qt::white);
        maskPainter.setBrush(QColor(0, 0, 0));
        maskPainter.drawRoundedRect(0, 0, m_headPhotoSize.width(), m_headPhotoSize.height(), 100, 100);
        m_headPhotoRound = new QPixmap(m_headPhoto->scaled(m_headPhotoSize));
        m_headPhotoRound->setMask(mask);
    }

    if(m_headPhotoHighlight != nullptr)
        m_headPhotoHighlightSize = {this->size().width()*65/98, this->size().height()*65/98};

    return QLabel::resizeEvent(event);
}

void HeadLabel::enterEvent(QEvent *e)
{
    if(tag == 1){
        mouseIn = true;
        setCursor(Qt::PointingHandCursor);

        m_headPhotoHighlight = new QPixmap(":/loginwnd/head_bkg_highlight");
        this->update();
    }
    return QLabel::enterEvent(e);
}

void HeadLabel::leaveEvent(QEvent *e)
{
    if(tag == 1){
        mouseIn = false;
        setCursor(Qt::ArrowCursor);
        m_headPhotoHighlight = new QPixmap(headPhotoHighlightPath);
        this->update();
    }
    return QLabel::enterEvent(e);
}

void HeadLabel::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton){
        emit clicked();
    }
}
