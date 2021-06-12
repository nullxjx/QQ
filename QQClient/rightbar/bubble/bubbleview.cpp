#include "bubbleview.h"

#include <QFontMetrics>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>

BubbleView::BubbleView(QWidget *parent,BubbleInfo *info)
    : QWidget(parent),info(info)
{
    textFont = QFont("Microsoft YaHei", 10, 50, false);
    sysMsgFont = QFont("Microsoft YaHei", 9, 50, false);
    setFont(textFont);

    connect(info,SIGNAL(updateProgressBar(qint64,qint64)),
            this,SLOT(slotupdateProgressBar(qint64,qint64)));
    connect(info,SIGNAL(updatePopMenu()),this,SLOT(sltUpdatePopMenu()));

    if(info->sender != System){
        QStringList tmp;

        //qDebug() << info->headIcon ;
        tmp << info->headIcon << "" << "";
        headIcon = new MyButton(this,tmp,QSize(40,40),HeadBtn);
        if(info->sender == Me){
            headIcon->id = info->myID;
        }else if (info->sender == You) {
            headIcon->id = info->yourID;
        }
        headIcon->name = info->name;
        headIcon->head = info->headIcon;

        if(info->showAnimation){
            loadingMovie = new QMovie(this);
            loadingMovie->setFileName(":/Icons/MainWindow/loading.gif");
            loadingLabel = new QLabel(this);
            loadingLabel->setMovie(loadingMovie);
            loadingLabel->setFixedSize(16,16);
            loadingLabel->setAttribute(Qt::WA_TranslucentBackground,true);
            loadingLabel->setAutoFillBackground(false);
            loadingMovie->start();
        }

        if(info->showError){
            errorLabel = new QLabel(this);
            errorLabel->setPixmap(QPixmap(":/Icons/MainWindow/error.png").
                                  scaled(20,20,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
            errorLabel->setFixedSize(20,20);
        }
    }

    if(info->msgType == Files){
        progressBar = new RadiusProgressBar;
        if(info->sender == Me)
            progressBar->setTag(0);
        else if(info->sender == You)
            progressBar->setTag(1);
        progressBar->setFixedWidth(200);
        progressBar->setFixedHeight(40);

        downloadLabel = new QLabel("未下载");

        if(!info->showProgressBar){
            progressBar->setMaximum(100);
            progressBar->setValue(100);
            downloadLabel->setFixedHeight(0);
        }

        downloadLabel->setFixedWidth(200);
        downloadLabel->setFont(textFont);
        if(!info->showDownload){
            downloadLabel->setFixedHeight(0);
        }else{
            progressBar->setFixedHeight(0);
            downloadLabel->setFixedHeight(30);
        }

        QFileInfo fileInfo = QFileInfo(info->msg);
        QString fileType = fileInfo.suffix();
        QString iconPath;

        fileNameLabel = new QLabel(fileInfo.fileName());
        fileNameLabel->setFixedWidth(200);
        fileNameLabel->setFont(textFont);
        QString  tmpStr = fileNameLabel->fontMetrics().elidedText(fileNameLabel->text(),Qt::ElideRight,
                                                                  fileNameLabel->width(), Qt::TextShowMnemonic);
        //文字显示不下自动省略
        fileNameLabel->setText(tmpStr);

        if(fileType == "pdf"){
            iconPath = ":/Icons/FileType/pdf.png";
        }else if(fileType == "mp4" || fileType == "avi"){
            iconPath = ":/Icons/FileType/video.png";
        }else if(fileType == "docx"){
            iconPath = ":/Icons/FileType/word.png";
        }else if(fileType == "png" || fileType == "jpg"){
            iconPath = ":/Icons/FileType/picture.png";
        }else {
            iconPath = ":/Icons/FileType/file.png";
        }

        fileIconLabel = new QLabel;
        fileIconLabel->setPixmap(QPixmap(iconPath).
                                 scaled(80,80,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        fileIconLabel->setFixedSize(80,80);

        fileSizeLabel = new QLabel(QString("file size: ") + getFileSize(info->fileSize));
        fileSizeLabel->setFixedWidth(200);
        fileSizeLabel->setFont(textFont);

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(fileNameLabel);
        layout->addWidget(progressBar);
        layout->addWidget(downloadLabel);
        layout->addWidget(fileSizeLabel);

        QHBoxLayout *mainLayout = new QHBoxLayout(this);
        if(info->sender == You){
            mainLayout->addWidget(fileIconLabel);
            mainLayout->addLayout(layout);
            mainLayout->addStretch(1);
            mainLayout->setContentsMargins(10+10+40+20,10+20+20,0,10);
        }else if(info->sender == Me){
            mainLayout->addStretch(1);
            mainLayout->addLayout(layout);
            mainLayout->addWidget(fileIconLabel);
            mainLayout->setContentsMargins(0,10+20+20,10+10+40+20,10);
        }
    }else if(info->msgType == Picture){
        picture = new QLabel;

        picture->setScaledContents(true);
        QPixmap pixmap = QPixmap(info->msg);
        if(pixmap.isNull()){
            qDebug() << "cannot find picture:" << info->msg;
            pixmap = QPixmap(":/Icons/MainWindow/deleted.png");
            info->msg = ":/Icons/MainWindow/deleted.png";
        }
        //qDebug() << "picture size:" << pixmap.size();

        QSize pixSize = pixmap.size();
        int width = pixSize.width();
        int height = pixSize.height();

        int pictureWidth;
        int pictureHeight;

        if(height > width){//高度大于宽度
            pictureHeight = 150;//高度保持150，宽度按原比例缩放
            pictureWidth = int(double(pictureHeight)/double(height) * width);
        }else{
            pictureWidth = 150;//宽度保持150，高度按原比例缩放
            pictureHeight = int(double(pictureWidth)/double(width) * height);
        }

        picture->setPixmap(pixmap.scaled(pictureWidth,pictureHeight,
                                         Qt::KeepAspectRatio,Qt::SmoothTransformation));
        picture->setStyleSheet("border:2px solid #4ba4f2;border-radius:0px");
        picture->setScaledContents(true);

        QHBoxLayout *mainLayout = new QHBoxLayout(this);
        if(info->sender == You){
            mainLayout->addWidget(picture);
            mainLayout->addStretch(1);
            mainLayout->setContentsMargins(10+40+20,20+20,0,10);
        }else if(info->sender == Me){
            mainLayout->addStretch(1);
            mainLayout->addWidget(picture);
            mainLayout->setContentsMargins(0,20+20,10+40+20,10);
        }
    }
}

QSize BubbleView::setRect()
{
    //qDebug() << "Geometry:" << this->geometry();
    /*
     * 文件框大小270*130 file
     * 图片框大小150*90(宽度实际上是会变化的，width保持固定，按相同的比例缩放)  picture
     * 头像和气泡框之间的空白宽度10 Blank1
     * 头像大小40*40    headIconWH
     * 头像和边框之间的空白宽度20 Blank2
     * 气泡框顶部和头像顶部的空白宽度20 Blank3
    */
    int fileWidth = 300;
    int fileHeight = 100;
    int pictureWidth = 150;
    int pictureHeight = 150;
    int Blank1 = 10;//头像和气泡框之间的空白宽度
    int Blank2 = 20;//头像和边框之间的空白宽度
    int Blank3 = 20;//气泡框顶部和头像顶部的空白宽度
    int Blank4 = 10;//气泡边框和内边矩形的宽度
    int headIconWH = 40;


    if(info->sender == Me){
        iconRect = QRect(this->geometry().width() - Blank2 - headIconWH,Blank2,headIconWH,headIconWH);
        triangleRect = QRect(iconRect.x()-Blank1,iconRect.y()+Blank3+10,5,10);
        if(info->msgType == Files){
            frameRect = QRect(iconRect.x()-Blank1-fileWidth - Blank4*2,iconRect.y()+Blank3,
                              fileWidth + Blank4*2,fileHeight + Blank4*2);
        }else if(info->msgType == Picture){
            QPixmap pixmap(info->msg);
            QSize pixSize = pixmap.size();
            int width = pixSize.width();
            int height = pixSize.height();

            if(height > width){//高度大于宽度
                pictureHeight = 150;//高度保持150，宽度按原比例缩放
                pictureWidth = int(double(pictureHeight)/double(height) * width);
            }else{
                pictureWidth = 150;//宽度保持150，高度按原比例缩放
                pictureHeight = int(double(pictureWidth)/double(width) * height);
            }

            frameRect = QRect(iconRect.x()-Blank1-pictureWidth - Blank4*2,iconRect.y()+Blank3,
                              pictureWidth + Blank4*2,pictureHeight + Blank4*2);
        }else if(info->msgType == Text){
            QSize textSize = getRealString(info->msg);
            frameRect = QRect(iconRect.x()-Blank1-Blank4*2 - textSize.width(),iconRect.y()+Blank3,
                              textSize.width() + Blank4*2,textSize.height() + Blank4*2);
            textRect = QRect(frameRect.x() + Blank4,frameRect.y()+Blank4,textSize.width(),textSize.height());
        }
        height = frameRect.height() + (Blank2+Blank3);
    }else if(info->sender == You){
        iconRect = QRect(Blank2,Blank2,headIconWH,headIconWH);
        triangleRect = QRect(iconRect.x()+headIconWH+Blank1,iconRect.y()+Blank3+10,5,10);
        if(info->msgType == Files){
            frameRect = QRect(iconRect.x()+headIconWH+Blank1,iconRect.y()+Blank3,
                              fileWidth + Blank4*2,fileHeight + Blank4*2);
        }else if(info->msgType == Picture){
            QPixmap pixmap(info->msg);
            QSize pixSize = pixmap.size();
            int width = pixSize.width();
            int height = pixSize.height();

            if(height > width){//高度大于宽度
                pictureHeight = 150;//高度保持150，宽度按原比例缩放
                pictureWidth = int(double(pictureHeight)/double(height) * width);
            }else{
                pictureWidth = 150;//宽度保持150，高度按原比例缩放
                pictureHeight = int(double(pictureWidth)/double(width) * height);
            }

            frameRect = QRect(iconRect.x()+headIconWH+Blank1,iconRect.y()+Blank3,
                              pictureWidth + Blank4*2,pictureHeight + Blank4*2);
        }else if(info->msgType == Text){
            QSize textSize = getRealString(info->msg);
            frameRect = QRect(iconRect.x()+headIconWH+Blank1,iconRect.y()+Blank3,
                              textSize.width()+Blank4*2,textSize.height()+Blank4*2);
            textRect = QRect(frameRect.x()+Blank4,frameRect.y()+Blank4,textSize.width(),textSize.height());
        }
        height = frameRect.height() + (Blank2+Blank3);
    }else if(info->sender == System){
        QSize textSize = getRealString(info->msg);
        textRect = QRect(this->width()/2 - textSize.width()/2,10,textSize.width(),textSize.height());
        frameRect = QRect(textRect.x() - 5,textRect.y() - 3,textRect.width()+10,textRect.height()+7);
        height = frameRect.height() + 10;
    }

    return QSize(this->width(),height);
}

QSize BubbleView::getRealString(QString src)
{
    QFont font = this->font();
    if(info->sender == System)
        font = this->sysMsgFont;

    QFontMetrics fm(font);

    lineHeight = fm.lineSpacing();
    int cnt = src.count("\n");
    int maxWidth = 0;
    textWidth = this->width() - 150;


    if(cnt == 0){
        maxWidth = fm.width(src);
        QString value = src;
        if(maxWidth > textWidth){
            maxWidth = textWidth;
            int size = textWidth/fm.width(" ");
            int num = fm.width(value)/textWidth;
            num = (fm.width(value))/textWidth;
            cnt += num;
            QString tmp = "";
            for(int i = 0;i < num;i++){
                tmp += value.mid(i*size,(i+1)*size) + "\n";
            }
            //src.replace(value,tmp);
        }
    }else{
        for(int i = 0;i < cnt + 1;i++){
            QString value = src.split("\n").at(i);
            maxWidth = fm.width(value) > maxWidth ? fm.width(value) : maxWidth;
            if(fm.width(value) > textWidth){
                maxWidth = textWidth;
                int size = textWidth/fm.width(" ");
                int num = fm.width(value)/textWidth;
                num = ((i+num)*fm.width(" ") + fm.width(value))/textWidth;
                cnt += num;
                QString tmp = "";
                for(int j = 0;j < num;j++){
                    tmp += value.mid(i*size,(i+1)*size) + "\n";
                }
                //src.replace(value,tmp);
            }
        }
    }
    return QSize(maxWidth,(cnt+1)*lineHeight);
}

QString BubbleView::getFileSize(qint64 size)
{
    if(size < 1024){//字节
        return QString::number(size) + " bytes";
    }else if(size >= 1024 && size < 1024*1024){//KB
        double size_ = static_cast<double>(size)/1024;
        return QString::number(size_,'f',1) + " KB";
    }else if(size >= 1024*1024 && size < 1024*1024*1024){
        double size_ = static_cast<double>(size)/(1024*1024);
        return QString::number(size_,'f',1) + " MB";
    }else if(size >= 1024*1024*1024 && size < qint64(1024)*1024*1024*1024){
        double size_ = static_cast<double>(size)/(1024*1024*1024);
        return QString::number(size_,'f',1) + " GB";
    }else{
        return "";
    }
}

void BubbleView::slotupdateProgressBar(qint64 sent, qint64 total)
{
    if(info->msgType == Files){
        if(info->showProgressBar){
            downloadLabel->setFixedHeight(0);
            progressBar->setFixedHeight(40);
            progressBar->setMaximum(100);

            if(sent >= total)
                sent = total;

            progressBar->setValue(int( double(sent) / double(total) * 100 ));

        }else{
            progressBar->setMaximum(100);
            progressBar->setValue(100);
        }
    }
}

void BubbleView::sltUpdatePopMenu()
{
    popMenu->clear();
    popMenu->addAction(tr("打开文件所在文件夹"));
    popMenu->addAction(tr("清屏"));
}

void BubbleView::sltHideAnimation()
{
    update();
}

void BubbleView::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::gray));

    if(info->sender == Me || info->sender == You){//自己发送的消息
        //画头像
        //painter.drawPixmap(iconRect,headIcon);
        headIcon->setGeometry(iconRect.x(),iconRect.y(),iconRect.width(),iconRect.height());

        if(info->sender == Me){
            QColor color_frame("#abcee5");
            painter.setBrush(QBrush(color_frame));

        }else{
            QColor color_frame(Qt::white);
            painter.setBrush(QBrush(color_frame));
        }

        //画边框
        if(info->msgType == Files || info->msgType == Text){
            painter.drawRoundedRect(frameRect,13,13);
        }

        //画三角
        if(info->sender == Me){
            QPointF points[3] = {
                QPointF(triangleRect.x(),triangleRect.y()),
                QPointF(triangleRect.x()+triangleRect.width(),triangleRect.y()+triangleRect.height()/2),
                QPointF(triangleRect.x(),triangleRect.y()+triangleRect.height())
            };
            QColor color_frame("#abcee5");
            QPen pen;
            pen.setColor(color_frame);
            painter.setPen(pen);
            painter.drawPolygon(points,3);

            if(info->showAnimation)
                loadingLabel->setGeometry(frameRect.x()-20,frameRect.y() + frameRect.height()/2 - 8,16,16);

            if(info->showError)
                errorLabel->setGeometry(frameRect.x()-20,frameRect.y() + frameRect.height()/2 - 8,20,20);
        }else{
            QPointF points[3] = {
                QPointF(triangleRect.x(),triangleRect.y()),
                QPointF(triangleRect.x()-triangleRect.width(),triangleRect.y()+triangleRect.height()/2),
                QPointF(triangleRect.x(),triangleRect.y()+triangleRect.height())
            };
            QColor color_frame(Qt::white);
            QPen pen;
            pen.setColor(color_frame);
            painter.setPen(pen);
            painter.drawPolygon(points,3);

            if(info->showAnimation)
                loadingLabel->setGeometry(frameRect.x()+frameRect.width()+4,frameRect.y() + frameRect.height()/2 - 8,16,16);

            if(info->showError)
                errorLabel->setGeometry(frameRect.x()-20,frameRect.y() + frameRect.height()/2 - 8,20,20);
        }

        if(info->msgType == Text){//普通文本消息
            //画文本内容
            QPen pen_text;
            pen_text.setColor(Qt::black);
            painter.setPen(pen_text);
            QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
            option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
            painter.setFont(this->font());
            painter.drawText(textRect,info->msg,option);

        }else if(info->msgType == Picture){//图片
            //            if(info->sender == Me)
            //                picture->setGeometry(iconRect.x()- 10 - picture->width(),iconRect.y()+20,
            //                                 picture->width(),picture->height());

        }else if(info->msgType == Files){//文件消息
            if(info->sender == Me){
                fileNameLabel->setStyleSheet("background-color:#abcee5;");
                fileIconLabel->setStyleSheet("background-color:#abcee5;");
                fileSizeLabel->setStyleSheet("background-color:#abcee5;");
                downloadLabel->setStyleSheet("background-color:#abcee5;");
            }else{
                fileNameLabel->setStyleSheet("background-color:white;");
                fileIconLabel->setStyleSheet("background-color:white;");
                fileSizeLabel->setStyleSheet("background-color:white;");
                downloadLabel->setStyleSheet("background-color:white;");
            }

        }
    }else if(info->sender == System){//系统发送的消息
        QColor color_frame("#dadada");
        painter.setBrush(QBrush(color_frame));
        painter.drawRoundedRect(frameRect,3,3);

        QPen pen_text;
        pen_text.setColor(Qt::white);
        painter.setPen(pen_text);
        QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
        option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        painter.setFont(this->sysMsgFont);
        painter.drawText(textRect,info->msg,option);
    }
}

void BubbleView::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton){
        if(info == nullptr || popMenu == nullptr) return;
        if(frameRect.contains(e->pos())){
            //qDebug() << frameRect << " | " << e->pos();
            emit mouseRightClicked(info,popMenu);
            popMenu->exec(QCursor::pos());
        }
    }
}

void BubbleView::enterEvent(QEvent *e)
{
    QMouseEvent *ev = static_cast<QMouseEvent *>(e);
    //qDebug() << frameRect << "  " << ev->pos();
    if(frameRect.contains(ev->pos())){
        setCursor(Qt::PointingHandCursor);
    }
    return QWidget::enterEvent(e);
}

void BubbleView::leaveEvent(QEvent *e)
{
    setCursor(Qt::ArrowCursor);
    return QWidget::leaveEvent(e);
}
