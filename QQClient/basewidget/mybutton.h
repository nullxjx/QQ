#ifndef LEFTBARBUTTON_H
#define LEFTBARBUTTON_H

#include <QPushButton>
#include <QWidget>
#include <QStringList>
#include <QMouseEvent>
#include <QEvent>
#include <QSize>

enum BtnType{
    NormalBtn,  //普通按钮
    HeadBtn     //头像按钮
};

class MyButton : public QPushButton
{   
    Q_OBJECT

public:
    MyButton(QWidget *parent,QStringList iconsList,QSize size,BtnType type = NormalBtn);
    void setImage(QString);
    void onBtnClicked();
    void restoreBtn();

    void changeIconSet(QStringList);

    QString NormalIcon;//正常显示时的图片
    QString MoveInIcon;//光标移入时的图片
    QString ClickIcon;//点击时的光标
    QSize btnSize;
    BtnType btnType;
    bool tag = false;

    //弹出个人信息界面中使用
    int id;
    QString name;
    int sex = 0;
    QString head;

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *event);

};

#endif // LEFTBARBUTTON_H

