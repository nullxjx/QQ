#ifndef ADDSUBGROUP_H
#define ADDSUBGROUP_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class AddSubGroup : public QDialog
{
    Q_OBJECT

public:
    AddSubGroup();

signals:
    void updateList(QString);

public slots:
    void sltBtnClicked();

private:
    QLabel *nameLabel;
    QLineEdit *nameEdit;

    QPushButton *okBtn;
    QPushButton *cancelBtn;
};

#endif // ADDSUBGROUP_H
