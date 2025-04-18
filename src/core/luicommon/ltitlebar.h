#ifndef LTITLEBAR_H
#define LTITLEBAR_H

#include "luicommon_global.h"
#include <QWidget>

class QHBoxLayout;
class QToolButton;
class QLabel;
/*
按钮布局
    |软件图标  | 软件名称（靠左）| 软件名称（居中）  |最小化   |最大化/还原     |关闭       |
    |m_pBtnLogo|m_pTitleLabelLeft|m_pTitleLabelCenter|m_pBtnMin|m_pBtnMaxOrNomal|m_pBtnClose|

*/

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	标题栏位置. </summary>
///
/// <remarks>	Liruilong, 2024/12/16. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TitlePos
{
    LeftPos = 0, // 靠左
    CenterPos    // 居中
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	自定义标题栏类. </summary>
///
/// <remarks>	Liruilong, 2024/12/16. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class LUICOMMON_EXPORT LTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit LTitleBar(QWidget * parent = nullptr);
    virtual ~LTitleBar() override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	logo按钮. </summary>
    ///
    /// <remarks>	Liruilong, 2024/12/16. </remarks>
    ///
    /// <returns>	Null if it fails, else a pointer to a QToolButton. </returns>
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    QToolButton * btnLogo();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	最小化. </summary>
    ///
    /// <remarks>	Liruilong, 2024/12/16. </remarks>
    ///
    /// <returns>	Null if it fails, else a pointer to a QToolButton. </returns>
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    QToolButton * btnMin();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	最大化或者正常. </summary>
    ///
    /// <remarks>	Liruilong, 2024/12/16. </remarks>
    ///
    /// <returns>	Null if it fails, else a pointer to a QToolButton. </returns>
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    QToolButton * btnMaxOrNomal();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	关闭. </summary>
    ///
    /// <remarks>	Liruilong, 2024/12/16. </remarks>
    ///
    /// <returns>	Null if it fails, else a pointer to a QToolButton. </returns>
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    QToolButton * btnClose();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	左侧标题栏. </summary>
    ///
    /// <remarks>	Liruilong, 2024/12/16. </remarks>
    ///
    /// <returns>	Null if it fails, else a pointer to a QLabel. </returns>
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    QLabel * labelLeft();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	右侧标题栏. </summary>
    ///
    /// <remarks>	Liruilong, 2024/12/16. </remarks>
    ///
    /// <returns>	Null if it fails, else a pointer to a QLabel. </returns>
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    QLabel * labelCenter();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	标题栏布局. </summary>
    /// <summary>	如果要插入一个窗口（控件），可以通过此方法，再根据下标获取插入位置. </summary>
    /// <summary>	如：QHBoxLayout * pLayout = qobject_cast<QHBoxLayout *>(widget->titleBar->titleLayout()); </summary>
    /// <summary>	    pLayout->insertWidget(2, new QWidget); // 插入到第三个按钮后面（m_pTitleLabelCenter 后，注意插入后的位置变动） </summary>
    ///
    /// <remarks>	Liruilong, 2024/12/16. </remarks>
    ///
    /// <returns>	Null if it fails, else a pointer to a QHBoxLayout. </returns>
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    QHBoxLayout * titleLayout();

    void setIcon(const QString & icon);
    void setCurrTitlePos(const TitlePos & pos = TitlePos::LeftPos);
    void setTitle(const QString & title);

signals:
    void btnMinClicked();
    void btnMaxOrNomalClicked();
    void btnCloseClicked();
    void titleBarDoubleClicked();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent * event) override;

private:
    QToolButton * m_pBtnLogo{nullptr};          // 软件图标
    QLabel *      m_pTitleLabelLeft{nullptr};   // 软件名称（靠左）
    QLabel *      m_pTitleLabelCenter{nullptr}; // 软件名称（居中）
    QToolButton * m_pBtnMin{nullptr};           // 最小化
    QToolButton * m_pBtnMaxOrNomal{nullptr};    // 最大化/还原
    QToolButton * m_pBtnClose{nullptr};         // 关闭

    QHBoxLayout * m_pTitleLayout{nullptr}; // 水平布局

};

#endif