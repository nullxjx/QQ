#include "mainui.h"
#include "myapp.h"
#include "database.h"

#include <QApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QImage>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "服务器启动..." << endl;

    //初始化程序
    MyApp::InitApp(a.applicationDirPath());
    qDebug() << "初始化路径完毕，当前路径为:" << a.applicationDirPath();

    // 加载数据库
    DataBase::Instance()->openDb(MyApp::m_strDatabasePath + "server.db");

    //显示主界面
    MainUI w;
    //w.show();

    return a.exec();
}
