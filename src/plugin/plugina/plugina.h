
#ifndef PLUGINA_H
#define PLUGINA_H

#include <QObject>
#include <QtPlugin>
#include <QAction>
#include <common/IPlugin.h>

class TestClass;

class PluginA : public QObject,
                public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PluginA" FILE "plugin_info.json") // QT5.0 引入
    Q_INTERFACES(IPlugin)
public:
    explicit PluginA(QObject * parent = 0);
    virtual QString getname() override;
    virtual bool    init() override;
    virtual bool    clean() override;

private:
    QAction * m_actionTestClass{nullptr}; // 按钮
};

#endif
