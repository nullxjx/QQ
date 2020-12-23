#ifndef IPLINEEDIT_H
#define IPLINEEDIT_H

#include <QLineEdit>
#include <QEvent>

class IPLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    IPLineEdit(QWidget *parent = nullptr);
    ~IPLineEdit();

    void setText(const QString&);
    QString getText() const;

protected:
    bool eventFilter(QObject *, QEvent *);
    int getIndex(QLineEdit*);
    bool isTextValid(const QString&);

private:
    QLineEdit *lineEdit[4];
};

#endif // IPLINEEDIT_H
