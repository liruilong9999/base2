#include "ldialog.h"

#include <QLayout>
#include <QEventLoop>

LDialog::LDialog(QWidget * parent)
{
    setParent(parent);
    // 设置窗口的模态化，达到打开其它窗口原窗口无法操作的目的
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    m_result = QDialog::Rejected;
    connect(this, &LDialog::accepted, this, &LDialog::accept);
    connect(this, &LDialog::rejected, this, &LDialog::reject);
    connect(this, &LDialog::finished, this, &LDialog::done);
}

LDialog::~LDialog()
{
}

void LDialog::setContentDialog(QDialog * diaLog)
{
    if (diaLog)
    {
        QLayout * layout = diaLog->layout();
        if (layout)
        {
            // 移除 diaLog 的布局，防止 diaLog 丢失布局
            diaLog->setLayout(nullptr);
            // 将布局的父对象设置为 this
            layout->setParent(this);
            // 将布局设置为 this 的布局
            this->setLayout(layout);
            // 设置布局的边距
            layout->setContentsMargins(0, 0, 0, 0);
            // 设置布局的大小约束
            layout->setSizeConstraint(QLayout::SetFixedSize);

            layout->addWidget(diaLog);
        }
    }
}

QDialog * LDialog::takeContentDialog()
{
    if (layout() && layout()->count() > 0)
    {
        QLayoutItem * item = layout()->takeAt(0);
        if (item)
        {
            QDialog * diaLog = qobject_cast<QDialog *>(item->widget());
            if (diaLog)
            {
                item->widget()->setParent(nullptr);
                delete item;
                return diaLog;
            }
        }
    }
    return nullptr;
}

QDialog * LDialog::contentDialog() const
{
    if (layout() && layout()->count() > 0)
    {
        QLayoutItem * item = layout()->takeAt(0);
        if (item)
        {
            QDialog * diaLog = qobject_cast<QDialog *>(item->widget());
            if (diaLog)
            {
                return diaLog;
            }
        }
    }
    return nullptr;
}

int LDialog::result() const
{
    return m_result;
}

void LDialog::setResult(int result)
{
    m_result = result;
    result ? emit accepted() : emit rejected();
}

void LDialog::accept()
{
    setResult(QDialog::Accepted);
    done(QDialog::Accepted);
}

void LDialog::reject()
{
    setResult(QDialog::Rejected);
    done(QDialog::Rejected);
}

void LDialog::exec()
{
    show();
    QEventLoop loop;
    connect(this, &LDialog::finished, &loop, &QEventLoop::quit);
    loop.exec(QEventLoop::DialogExec);
}

void LDialog::open()
{
    setResult(QDialog::Rejected);
    show();
}

void LDialog::close()
{
    hide();
    done(QDialog::Rejected);
}

void LDialog::done(int result)
{
    setResult(result);
    hide();
    emit finished(result);
}

void LDialog::closeEvent(QCloseEvent * event)
{
    QWidget::closeEvent(event);
}
