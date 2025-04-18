#include "ltitlebar.h"
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QMouseEvent>
#include <QIcon>
#include <QApplication>

LTitleBar::LTitleBar(QWidget * parent)
    : QWidget(parent)
    , m_pBtnLogo(new QToolButton(this))
    , m_pBtnMin(new QToolButton(this))
    , m_pBtnMaxOrNomal(new QToolButton(this))
    , m_pBtnClose(new QToolButton(this))
    , m_pTitleLabelLeft(new QLabel(this))
    , m_pTitleLabelCenter(new QLabel(this))
    , m_pTitleLayout(new QHBoxLayout(this))
{
    // Qt::FramelessWindowHint 无边框
    // Qt::WindowStaysOnTopHint 窗口在最顶端，不会拖到任务栏下面
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);

    // 设置按钮和标签的样式
    m_pBtnLogo->setFixedSize(24, 24);
    m_pBtnMin->setFixedSize(24, 24);
    m_pBtnMaxOrNomal->setFixedSize(24, 24);
    m_pBtnClose->setFixedSize(24, 24);

    m_pBtnMin->setToolTip("Minimize");
    m_pBtnMaxOrNomal->setToolTip("Maximize/Restore");
    m_pBtnClose->setToolTip("Close");

    m_pBtnLogo->setIcon(QIcon(":/icons/logo"));
    m_pBtnMin->setIcon(QIcon(":/icons/minimize"));
    m_pBtnMaxOrNomal->setIcon(QIcon(":/icons/maximize"));
    m_pBtnClose->setIcon(QIcon(":/icons/close"));

    m_pTitleLabelLeft->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pTitleLabelCenter->setAlignment(Qt::AlignCenter);

    // 布局设置
    m_pTitleLayout->addWidget(m_pBtnLogo);
    m_pTitleLayout->addWidget(m_pTitleLabelLeft);
    m_pTitleLayout->addStretch();
    m_pTitleLayout->addWidget(m_pTitleLabelCenter);
    m_pTitleLayout->addStretch();
    m_pTitleLayout->addWidget(m_pBtnMin);
    m_pTitleLayout->addWidget(m_pBtnMaxOrNomal);
    m_pTitleLayout->addWidget(m_pBtnClose);

    m_pTitleLayout->setContentsMargins(0, 0, 0, 0);
    m_pTitleLayout->setSpacing(5);
    setLayout(m_pTitleLayout);

    // 检查是否存在 QSS 文件
    QFile qssFile(":/styles/default.css");
    if (qssFile.exists() && qssFile.open(QFile::ReadOnly))
    {
        QString qssContent = qssFile.readAll();
        qApp->setStyleSheet(qssContent);
        qssFile.close();
    }
    // 信号与信号连接
    connect(m_pBtnMin, &QToolButton::clicked, this, &LTitleBar::btnMinClicked);
    connect(m_pBtnMaxOrNomal, &QToolButton::clicked, this, &LTitleBar::btnMaxOrNomalClicked);
    connect(m_pBtnClose, &QToolButton::clicked, this, &LTitleBar::btnCloseClicked);

    setCurrTitlePos(); // 默认执行一次

    setStyleSheet("background-color: #ffffff;");
}

LTitleBar::~LTitleBar()
{
    // Qt 的父子关系会自动清理内存，无需手动删除
}

QToolButton * LTitleBar::btnLogo()
{
    return m_pBtnLogo;
}

QToolButton * LTitleBar::btnMin()
{
    return m_pBtnMin;
}

QToolButton * LTitleBar::btnMaxOrNomal()
{
    return m_pBtnMaxOrNomal;
}

QToolButton * LTitleBar::btnClose()
{
    return m_pBtnClose;
}

QLabel * LTitleBar::labelLeft()
{
    return m_pTitleLabelLeft;
}

QLabel * LTitleBar::labelCenter()
{
    return m_pTitleLabelCenter;
}

QHBoxLayout * LTitleBar::titleLayout()
{
    return m_pTitleLayout;
}

void LTitleBar::setIcon(const QString & icon)
{
    m_pBtnLogo->setIcon(QIcon(icon));
}

void LTitleBar::setCurrTitlePos(const TitlePos & pos)
{
    if (pos == TitlePos::CenterPos)
    {
        m_pTitleLabelCenter->setVisible(true);
        m_pTitleLabelLeft->setVisible(false);
    }
    else
    {
        m_pTitleLabelCenter->setVisible(false);
        m_pTitleLabelLeft->setVisible(true);
    }
}

void LTitleBar::setTitle(const QString & title)
{
    m_pTitleLabelLeft->setText(title);
    m_pTitleLabelCenter->setText(title);
}

void LTitleBar::mouseDoubleClickEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit titleBarDoubleClicked();
    }
}
