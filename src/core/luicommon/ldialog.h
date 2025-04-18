#ifndef LDIALOG_H
#define LDIALOG_H

#include "luicommon_global.h"
#include <QWidget>
#include <QDialog>

#include "lwidget.h"

class QEventLoop;
class QDialog;

class LUICOMMON_EXPORT LDialog : public LWidget
{
    Q_OBJECT
public:
    explicit LDialog(QWidget * parent = nullptr);
    virtual ~LDialog();

    void      setContentDialog(QDialog * diaLog);
    QDialog * takeContentDialog();
    QDialog * contentDialog() const;
    int       result() const; // 0: rejected, 1: accepted, 2: closed
    void      setResult(int result); // 0: rejected, 1: accepted, 2: closed

signals:
    void accepted();
    void rejected();
    void finished(int result); // 0:表示 reject, 1:表示 accept, 2:表示 close

public slots:
    virtual void accept();
    virtual void reject();
    virtual void close();
    virtual void exec();
    virtual void open();
    virtual void done(int result);

protected:
    virtual void closeEvent(QCloseEvent * event) override;


private:
    int m_result;
};

#endif
