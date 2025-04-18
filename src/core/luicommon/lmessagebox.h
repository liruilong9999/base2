#ifndef LMESSAGEBOX_H
#define LMESSAGEBOX_H

#include "ldialog.h"

class LMessageBox : public LDialog
{
public:
    LMessageBox(const QString & title, const QString & message, const QString & button1Text, const QString & button2Text = "", const QString & button3Text = "");
    ~LMessageBox();

    enum StandardButton
    {
        // keep this in sync with QDialogButtonBox::StandardButton and QPlatformDialogHelper::StandardButton
        NoButton        = 0x00000000,
        Ok              = 0x00000400,
        Save            = 0x00000800,
        SaveAll         = 0x00001000,
        Open            = 0x00002000,
        Yes             = 0x00004000,
        YesToAll        = 0x00008000,
        No              = 0x00010000,
        NoToAll         = 0x00020000,
        Abort           = 0x00040000,
        Retry           = 0x00080000,
        Ignore          = 0x00100000,
        Close           = 0x00200000,
        Cancel          = 0x00400000,
        Discard         = 0x00800000,
        Help            = 0x01000000,
        Apply           = 0x02000000,
        Reset           = 0x04000000,
        RestoreDefaults = 0x08000000,

        FirstButton = Ok,              // internal
        LastButton  = RestoreDefaults, // internal

        YesAll = YesToAll, // obsolete
        NoAll  = NoToAll,  // obsolete

        Default    = 0x00000100, // obsolete
        Escape     = 0x00000200, // obsolete
        FlagMask   = 0x00000300, // obsolete
        ButtonMask = ~FlagMask   // obsolete
    };
    typedef StandardButton Button; // obsolete

    Q_DECLARE_FLAGS(StandardButtons, StandardButton) // 这里是 Qt5新增的宏定义，用于声明一个枚举类型，并提供一些便利的宏定义。
    Q_FLAG(StandardButtons)                          // 这里是 Qt 5新增的宏定义，用于声明一个枚举类型，并提供一些便利的宏定义。

    static StandardButton information(QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton);
    static StandardButton question(QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = StandardButtons(Yes | No), StandardButton defaultButton = NoButton);
    static StandardButton warning(QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton);
    static StandardButton critical(QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton);

    void setTitile(const QString & title);
    void setIcon(const QIcon & icon);
    void setIcon(const QPixmap & pix);
    void setText(const QString & text);

    void setStardardButtons(StandardButtons buttons);
    void setDefaultButton(StandardButton button);

    StandardButton clickedButton() const;

    virtual QSize sizeHint() const override;

protected:
    //virtual void setupUi() override;

private:
    // void initUI();

private:
    QString m_title;
    QString m_message;
    QString m_button1Text;
    QString m_button2Text;
    QString m_button3Text;
};

#endif