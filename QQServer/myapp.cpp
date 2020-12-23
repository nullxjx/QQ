#include "myapp.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QDebug>

QString MyApp::m_strAppPath         = "./";     // 应用程序路径，即exe文件所在目录
QString MyApp::m_strDataPath        = "";       // 数据保存路径
QString MyApp::m_strDatabasePath    = "";       // 数据库目录
QString MyApp::m_strConfPath        = "";       // 配置目录
QString MyApp::m_strBackupPath      = "";
QString MyApp::m_strRecvPath        = "";
QString MyApp::m_strHeadPath        = "";

QString MyApp::m_strIniFile         = "config.ini"; // 配置文件
QString MyApp::m_strUserName        = "admin";
QString MyApp::m_strPassword        = "123456";

int     MyApp::m_nId                = -1;
int     MyApp::m_nIdentyfi          = -1;

// 初始化
void MyApp::InitApp(const QString &appPath)
{
    m_strAppPath        = appPath + "/";
    m_strDataPath       = m_strAppPath  + "Data/";
    m_strDatabasePath   = m_strDataPath + "Database/";
    m_strConfPath       = m_strDataPath + "Conf/";
    m_strBackupPath     = m_strDataPath + "Backup/";
    m_strRecvPath       = m_strDataPath + "Files/";
    m_strHeadPath       = m_strDataPath + "Head/";
    m_strIniFile        = m_strConfPath + "config.ini";

    // 检查目录
    CheckDirs();

    // 创建配置文件
    CreateSettingFile();

    // 加载系统配置
    ReadSettingFile();
}

void MyApp::CreateSettingFile() {
    // 写入配置文件
    QSettings settings(m_strIniFile, QSettings::IniFormat);
    QString strGroups = settings.childGroups().join("");
    if (!QFile::exists(m_strIniFile) || (strGroups.isEmpty()))
    {

        /*系统设置*/
        settings.beginGroup("UserCfg");
        settings.setValue("User",   m_strUserName);
        settings.setValue("Passwd", m_strPassword);
        settings.endGroup();
        settings.sync();

    }
#ifdef Q_WS_QWS
    QProcess::execute("sync");
#endif
}

void MyApp::ReadSettingFile()
{
    QSettings settings(m_strIniFile, QSettings::IniFormat);
    settings.beginGroup("UserCfg");
    m_strUserName = settings.value("User", "admin").toString();
    m_strPassword = settings.value("Passwd", "123456")  .toString();
    settings.endGroup();
}

void MyApp::SetSettingFile(const QString &group, const QString &key, const QVariant &value)
{
    QSettings settings(m_strIniFile, QSettings::IniFormat);
    settings.beginGroup(group);
    settings.setValue(key, value);
    settings.sync();
}

QVariant MyApp::GetSettingKeyValue(const QString &group, const QString &key, const QVariant &value)
{
    QSettings settings(m_strIniFile, QSettings::IniFormat);
    settings.beginGroup(group);
    return settings.value(key, value);
}

void MyApp::CheckDirs()
{
    // 数据文件夹
    QDir dir(m_strDataPath);
    if (!dir.exists()) {
        dir.mkdir(m_strDataPath);
#ifdef Q_WS_QWS
        QProcess::execute("sync");
#endif
    }

    // 数据库目录
    dir.setPath(m_strDatabasePath);
    if (!dir.exists()) {
        dir.mkdir(m_strDatabasePath);
#ifdef Q_WS_QWS
        QProcess::execute("sync");
#endif
    }

    // 备份文件目录
    dir.setPath(m_strBackupPath);
    if (!dir.exists()) {
        dir.mkdir(m_strBackupPath);
#ifdef Q_WS_QWS
        QProcess::execute("sync");
#endif
    }

    // 服务器文件目录
    dir.setPath(m_strRecvPath);
    if (!dir.exists()) {
        dir.mkdir(m_strRecvPath);
#ifdef Q_WS_QWS
        QProcess::execute("sync");
#endif
    }

    // 头像目录
    dir.setPath(m_strHeadPath);
    if (!dir.exists()) {
        dir.mkdir(m_strHeadPath);
#ifdef Q_WS_QWS
        QProcess::execute("sync");
#endif
    }
}

void MyApp::SaveConfig()
{
    QSettings settings(m_strIniFile, QSettings::IniFormat);

    /*系统设置*/
    settings.beginGroup("UserCfg");
    settings.setValue("User",   m_strUserName);
    settings.setValue("Passwd", m_strPassword);
    settings.endGroup();

    settings.sync();
}

void MyApp::createDir(QString path)
{
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkdir(path);
#ifdef Q_WS_QWS
        QProcess::execute("sync");
#endif
    }
}
