#include "global.h"
#include "myapp.h"
#include "database.h"
#include "rotatingstackedwidget.h"

#include <QApplication>
#include <QTextCodec>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //a.setQuitOnLastWindowClosed(false);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));

    MyApp::initApp(a.applicationDirPath());
    myHelper::setStyle("default");

    // 加载数据库
    DataBase::Instance()->openDb(MyApp::m_strDatabasePath + "user.db");

    //显示登陆界面
    RotatingStackedWidget launcher;
    launcher.show();

    return a.exec();
}
