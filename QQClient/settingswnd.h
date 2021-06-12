#ifndef SETTINGSWND_H
#define SETTINGSWND_H

#include <QDialog>
#include <QLabel>
#include <QCheckBox>

class SettingsWnd : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsWnd(QWidget *parent = nullptr);

signals:

public slots:
    void setAutoLogin(int);

private:
    QLabel *autoLoginLabel;
    QCheckBox *autoLoginCheckBox;
};

#endif // SETTINGSWND_H
