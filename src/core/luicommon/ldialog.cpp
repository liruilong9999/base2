#include "ldialog.h"

#include <QLayout>
#include <QEventLoop>

LDialog::LDialog(QWidget * parent)
{
    setParent(parent);
    // ���ô��ڵ�ģ̬�����ﵽ����������ԭ�����޷�������Ŀ��
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
            // �Ƴ� diaLog �Ĳ��֣���ֹ diaLog ��ʧ����
            diaLog->setLayout(nullptr);
            // �����ֵĸ���������Ϊ this
            layout->setParent(this);
            // ����������Ϊ this �Ĳ���
            this->setLayout(layout);
            // ���ò��ֵı߾�
            layout->setContentsMargins(0, 0, 0, 0);
            // ���ò��ֵĴ�СԼ��
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
