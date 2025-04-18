#ifndef PLUGININTERFACE__H__
#define PLUGININTERFACE__H__

#include <QString>
#include <QObject>

class IPlugin
{
public:
    // 虚析构，保障内存上先释放子类
    virtual ~IPlugin(){};

    virtual QString getname() = 0;
    virtual bool    init()    = 0;
    virtual bool    clean()   = 0;
};

#define IPlugin_iid "lrl.QtPluginsManager.IPlugin"
Q_DECLARE_INTERFACE(IPlugin, IPlugin_iid)

#endif
