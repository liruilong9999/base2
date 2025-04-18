#ifndef LMAINWINDOW_H
#define LMAINWINDOW_H
#include "luicommon_global.h"
#include "lwidget.h"
#include <QMainWindow>
#include <QPushButton>

class LUICOMMON_EXPORT LMainwindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit LMainwindow(QWidget * parent = nullptr);
    virtual ~LMainwindow() override;

    void show();
    void showLMainWindow();

    LWidget * lwidget() const;
signals:

public slots:

private:
    LWidget * _pLWidget = nullptr;
};

#endif