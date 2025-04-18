#include <QDebug>
#include <QAction>
#include <QLabel>
#include <QStatusBar>
#include <QThread>


#include "pluginb.h"

PluginB::PluginB(QObject * parent)
    : QObject(parent)
{
}

QString PluginB::getname()
{
    return QString("插件B");
}

bool PluginB::init()
{
    return true;
}

bool PluginB::clean()
{
    return true;
}