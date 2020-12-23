#ifndef MYAPP_H
#define MYAPP_H

#include <QObject>

#if 0
typedef enum {
    Administrator = 0x01,
    Mangager,
    Worker
} E_IDENTITY;
#endif

class QApplication;

class MyApp
{
public:
    //=======================系统配置部分=========================//
    static QString m_strAppPath;         // 应用程序路径
    static QString m_strDataPath;        // 数据保存路径
    static QString m_strDatabasePath;    // 数据库目录
    static QString m_strConfPath;        // 配置目录
    static QString m_strBackupPath;      // 配置目录
    static QString m_strRecvPath;        // 文件接收保存目录
    static QString m_strHeadPath;        // 用户头像目录

    static QString m_strIniFile;         // 配置文件

    static QString m_strUserName;        // 用户名
    static QString m_strPassword;        // 用户密码

    static int     m_nId;
    static int     m_nIdentyfi;

    //=======================函数功能部分=========================//
    // 初始化
    static void InitApp(const QString &appPath);
    // 创建配置文件
    static void CreateSettingFile();
    // 读配置文件，加载系统配置
    static void ReadSettingFile();

    // 读取/修改配置文件
    static void SetSettingFile(const QString &group, const QString &key, const QVariant &value);
    static QVariant GetSettingKeyValue(const QString &group, const QString &key, const QVariant &value);
    // 检查目录
    static void CheckDirs();

    static void SaveConfig();

    static void createDir(QString path);
};

#endif // MYAPP_H
