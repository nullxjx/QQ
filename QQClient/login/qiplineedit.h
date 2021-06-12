#ifndef QIPLINEEDIT_H
#define QIPLINEEDIT_H

#include <QLineEdit>
#include <QEvent>

class QLabel;

class QIPLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    QIPLineEdit(QWidget *parent = nullptr);
    ~QIPLineEdit();

    void setText(const QString &strIP);
    QString getText() const;

signals:
    void textChanged();

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

    int getIndex(QLineEdit *pEdit);
    bool isTextValid(const QString &strIP);

private:
    QLineEdit *m_lineEidt[4];
};

/////////////////////////////////////////////
class QMacLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    QMacLineEdit(QWidget *parent = nullptr);
    ~QMacLineEdit();

    void setText(const QString &strMac);
    QString text() const;
protected:
//    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *obj, QEvent *ev);

    int getIndex(QLineEdit *pEdit);
    bool isTextValid(const QString &strIP);
private:
    QLineEdit *m_lineEidt[6];
};

///////////////////////////////////////////////////////////
/// \brief QIconLineEdit::QIconLineEdit
/// \param parent
/// 带图片的linedit
class QIconLineEdit : public QLineEdit {
    Q_OBJECT
public:
    QIconLineEdit(QWidget *parent = nullptr);
    ~QIconLineEdit();

    void SetIcon(const QPixmap &pixmap);
private:
    QLabel *labelPixmap;
};

#endif // QIPLINEEDIT_H

