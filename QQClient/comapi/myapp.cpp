#include "myapp.h"
#include "unit.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QDebug>

// 应用程序配置目录
QString MyApp::m_strAppPath         = "./";
QString MyApp::m_strDataPath        = "";
QString MyApp::m_strRecvPath        = "";
QString MyApp::m_strDatabasePath    = "";
QString MyApp::m_strConfPath        = "";
QString MyApp::m_strFacePath        = "";
QString MyApp::m_strHeadPath        = "";
QString MyApp::m_strSoundPath       = "";
QString MyApp::m_strRecordPath      = "";
// 配置文件
QString MyApp::m_strIniFile         = "config.ini";
QString MyApp::m_debugFile          = "debug.log";

// 服务器相关配置
QString MyApp::m_strHostAddr        = "";
int     MyApp::m_nMsgPort           = 0;
int     MyApp::m_nFilePort          = 0;
int     MyApp::m_nGroupPort         = 0;

//用户信息配置
int     MyApp::m_nId                = -1;
QString MyApp::m_strUserName        = "";
QString MyApp::m_strPassword        = "";
QString MyApp::m_strHeadFile        = "";

//应用程序设置
bool    MyApp::checked              = false;
bool    MyApp::autoLogin            = false;
bool    MyApp::sendWay              = false;
int     MyApp::m_nWinX              = 0;
int     MyApp::m_nWinY              = 0;


// 初始化
void MyApp::initApp(const QString &appPath)
{
    m_strAppPath        = appPath + "/";

    m_strDataPath       = m_strAppPath  + "Data/";
    m_strRecvPath       = m_strDataPath + "RecvFiles/";
    m_strDatabasePath   = m_strDataPath + "Database/";
    m_strConfPath       = m_strDataPath + "Conf/";
    m_strHeadPath       = m_strDataPath + "Head/";
    m_strSoundPath      = m_strDataPath + "Sound/";
    m_strRecordPath     = m_strDataPath + "Record/";
    m_strFacePath       = m_strDataPath + "Face/";
    m_strIniFile        = m_strConfPath + "config.ini";

    // 检查目录
    checkDirs();

    // 检测音频文件
    checkSound();

    // 创建配置文件
    createSettingFile();

    // 加载系统配置
    readSettingFile();

    checkDebugFile();
}


void MyApp::createSettingFile() {
    // 写入配置文件
    QSettings settings(m_strIniFile, QSettings::IniFormat);
    QString strGroups = settings.childGroups().join("");
    if (!QFile::exists(m_strIniFile) || (strGroups.isEmpty()))
    {
        /*用户信息设置*/
        settings.beginGroup("User");
        settings.setValue("ID", m_nId);
        settings.setValue("Name", m_strUserName);
        settings.setValue("Passwd", m_strPassword);
        settings.endGroup();

        /*其他配置*/
        settings.beginGroup("Server");
        settings.setValue("HostAddr", m_strHostAddr);
        settings.setValue("MsgPort",  m_nMsgPort);
        settings.setValue("FilePort",  m_nFilePort);
        settings.setValue("GroupPort",  m_nGroupPort);
        settings.endGroup();

        /*应用程序设置*/
        settings.beginGroup("Application");
        settings.setValue("checked", checked);
        settings.setValue("autoLogin", autoLogin);
        settings.setValue("sendWay", sendWay);
        settings.endGroup();

        settings.sync();

    }
#ifdef Q_WS_QWS
    QProcess::execute("sync");
#endif
}


// 保存配置
void MyApp::saveConfig()
{
    QSettings settings(m_strIniFile, QSettings::IniFormat);

    /*系统设置*/
    settings.beginGroup("User");
    settings.setValue("ID", m_nId);
    settings.setValue("Name", m_strUserName);
    settings.setValue("Passwd", m_strPassword);
    settings.endGroup();

    /*其他配置*/
    settings.beginGroup("Server");
    settings.setValue("HostAddr", m_strHostAddr);
    settings.setValue("MsgPort",  m_nMsgPort);
    settings.setValue("FilePort",  m_nFilePort);
    settings.setValue("GroupPort",  m_nGroupPort);
    settings.endGroup();

    /*应用程序设置*/
    settings.beginGroup("Application");
    settings.setValue("checked", checked);
    settings.setValue("autoLogin", autoLogin);
    settings.setValue("sendWay", sendWay);
    settings.endGroup();

    settings.sync();
}



void MyApp::readSettingFile()
{
    QSettings settings(m_strIniFile, QSettings::IniFormat);
    settings.beginGroup("User");
    m_nId = settings.value("ID").toInt();
    m_strUserName = settings.value("Name").toString();
    m_strPassword = settings.value("Passwd").toString();
    m_strHeadFile = QString::number(m_nId) + ".png";
    settings.endGroup();


    settings.beginGroup("Server");
    m_strHostAddr = settings.value("HostAddr").toString();
    m_nMsgPort    = settings.value("MsgPort").toInt();
    m_nFilePort   = settings.value("FilePort").toInt();
    m_nGroupPort  = settings.value("GroupPort").toInt();
    settings.endGroup();

    settings.beginGroup("Application");
    checked = settings.value("checked").toBool();
    autoLogin = settings.value("autoLogin").toBool();
    sendWay = settings.value("sendWay").toBool();
    settings.endGroup();
}


void MyApp::setSettingFile(const QString &group, const QString &key, const QVariant &value)
{
    QSettings settings(m_strIniFile, QSettings::IniFormat);
    settings.beginGroup(group);
    settings.setValue(key, value);
    settings.sync();
}


QVariant MyApp::getSettingKeyValue(const QString &group, const QString &key, const QVariant &value)
{
    QSettings settings(m_strIniFile, QSettings::IniFormat);
    settings.beginGroup(group);
    return settings.value(key, value);
}


void MyApp::checkDirs()
{
    // 数据文件夹
    QDir dir(m_strDataPath);
    if (!dir.exists()) {
        dir.mkdir(m_strDataPath);
#ifdef Q_WS_QWS
        QProcess::execute("sync");
#endif
    }


    // 接收文件目录
    dir.setPath(m_strRecvPath);
    if (!dir.exists()) {
        dir.mkdir(m_strRecvPath);
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

    // 配置文件目录
    dir.setPath(m_strConfPath);
    if (!dir.exists()) {
        dir.mkdir(m_strConfPath);
#ifdef Q_WS_QWS
        QProcess::execute("sync");
#endif
    }

    // 表情目录
    dir.setPath(m_strFacePath);
    if (!dir.exists()) {
        dir.mkdir(m_strFacePath);
#ifdef Q_WS_QWS
        QProcess::execute("sync");
#endif
    }

    // 头像检测目录
    dir.setPath(m_strHeadPath);
    if (!dir.exists()) {
        dir.mkdir(m_strHeadPath);
#ifdef Q_WS_QWS
        QProcess::execute("sync");
#endif
    }

    // 音频目录
    dir.setPath(m_strSoundPath);
    if (!dir.exists()) {
        dir.mkdir(m_strSoundPath);
#ifdef Q_WS_QWS
        QProcess::execute("sync");
#endif
    }
}


void MyApp::checkSound()
{
    if (!QFile::exists(MyApp::m_strSoundPath + "message.wav")) {
        QFile::copy(":/sound/resource/sound/message.wav", MyApp::m_strSoundPath + "message.wav");
    }

    if (!QFile::exists(MyApp::m_strSoundPath + "msg.wav")) {
        QFile::copy(":/sound/resource/sound/msg.wav", MyApp::m_strSoundPath + "msg.wav");
    }

    if (!QFile::exists(MyApp::m_strSoundPath + "ringin.wav")) {
        QFile::copy(":/sound/resource/sound/ringin.wav", MyApp::m_strSoundPath + "ringin.wav");
    }

    if (!QFile::exists(MyApp::m_strSoundPath + "system.wav")) {
        QFile::copy(":/sound/resource/sound/system.wav", MyApp::m_strSoundPath + "system.wav");
    }

    if (!QFile::exists(MyApp::m_strSoundPath + "userlogon.wav")) {
        QFile::copy(":/sound/resource/sound/userlogon.wav", MyApp::m_strSoundPath + "userlogon.wav");
    }
}

void MyApp::checkDebugFile()
{
    QFile file(MyApp::m_strConfPath + MyApp::m_debugFile);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug()<<"open failed!";
    }
    file.close();
}
