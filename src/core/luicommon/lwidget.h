#ifndef LWIDGET_H
#define LWIDGET_H

#include <QWidget>
#include <QPointer>
#include <QMouseEvent>
#include <QPainter>
#include "luicommon_global.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	窗口状态. </summary>
///
/// <remarks>	Liruilong, 2024/12/16. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class WidgetState
{
    MinState = 0, // 最小化
    MaxState,     // 最大化
    NomalState,   // 正常大小
    OtherState    // 其它大小
};

class LTitleBar;
class QIcon;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	实现一个自定义的QWidget. </summary>
///
/// <remarks>	Liruilong, 2024/12/16. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class LUICOMMON_EXPORT LWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LWidget(QWidget * parent = nullptr);
    virtual ~LWidget() override;

    void initWidget();

    void setWidgetState(WidgetState widgetState);
    void connectInit();

    void      setContentWidget(QWidget * pContentWidget);
    QWidget * contentWidget();
    QWidget * takeContentWidget();

    LTitleBar * titleBar();

    QWidget * palletWidget();

    void loadStyleSheet(const QString & stylePath);

public:
    void setWindowTitle(QString title);
    void setWindowIcon(QIcon icon);


public slots:
    void onTitleBarBtnCloseClicked();
    void onTitleBarBtnMinClicked();
    void onTitleBarBtnNomalOrMaxClicked();
    void onTitleBarDoubleClicked();

protected:
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void updateCursorShape(const QPoint & pos);

private:
    WidgetState m_currentWidgetState{WidgetState::MaxState};

    /*
    widget结构
     _____________________________
    |                             |
    |          标题栏             |
    |          工具栏			  |
    |          正文               |
    |_____________________________|
    */
    LTitleBar *       m_pTitleBar{nullptr};     // 标题栏
    QWidget *         m_pPalletWidget{nullptr}; // 工具栏
    QPointer<QWidget> m_pCenterWidget{nullptr}; // 正文
    QPoint            m_mousePos{0, 0};
    QPoint            m_normalPos;

    bool m_isMousePressed{false};

private:
    enum ResizeDirection
    {
        NoEdge = 0x0,
        Top    = 0x1,
        Bottom = 0x2,
        Left   = 0x4,
        Right  = 0x8
    };
    // 重绘界面大小的标志位
    bool   m_resizing{false};
    int    m_resizeDirection;
    QRect  m_windowRect;
    QPoint m_pressPos;

};

#endif