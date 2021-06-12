#include "addsubgroup.h"
#include "database.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QDateTime>

AddSubGroup::AddSubGroup()
{
    setWindowTitle("新建分组");
    setFixedSize(350,150);
    setWindowFlags(Qt::WindowCloseButtonHint);

    QFont font = QFont("Microsoft YaHei", 12, 50, false);
    QFont font2 = QFont("Microsoft YaHei", 10, 50, false);
    nameLabel = new QLabel("请输入分组名：",this);
    nameLabel->setFont(font);
    nameEdit = new QLineEdit(this);
    nameEdit->setFont(font2);

    okBtn = new QPushButton("确定",this);
    okBtn->setStyleSheet("QPushButton{border:1px solid #86949e;background-color:#0188fb;border-radius:5px}"
                          "QPushButton:hover{background-color:#289cff;}"
                          "QPushButton:pressed{background-color:#0081ef}");
    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(sltBtnClicked()));

    cancelBtn = new QPushButton("取消",this);
    cancelBtn->setStyleSheet("QPushButton{border:1px solid #86949e;background-color:#0188fb;border-radius:5px}"
                          "QPushButton:hover{background-color:#289cff;}"
                          "QPushButton:pressed{background-color:#0081ef}");

    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(close()));

    nameLabel->setGeometry(20,30,150,30);
    nameEdit->setGeometry(150,30,180,30);
    okBtn->setGeometry(250,100,80,30);
    cancelBtn->setGeometry(150,100,80,30);

    QPalette palette;
    palette.setColor(QPalette::Background, Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(palette);
}

void AddSubGroup::sltBtnClicked()
{
    QString text = nameEdit->text();
    if(text.size() > 20){
        QMessageBox::information(this,"error","分组名不能多于20个字符!");
        return;
    }else {
        QJsonArray jsonArray = DataBase::Instance()->getMySubgroup();
        int cnt = jsonArray.size();
        for(int i = 0;i < cnt;i++){
             QJsonObject json = jsonArray.at(i).toObject();
             QString name = json.value("name").toString();
             if(text == name){
                 QMessageBox::information(this,"error","分组中已存在该分组名!");
                 return;
             }
        }

        QSqlQuery query;
        QString sql = "insert into mySubgroup values('" + text + "','" +
                QDateTime::currentDateTime().toString("yyyy-MM-dd  hh:mm:ss") + "')";
        query.exec(sql);

        emit updateList(text);

        this->close();
    }
}
