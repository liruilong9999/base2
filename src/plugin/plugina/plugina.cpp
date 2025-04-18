#include <QDebug>
#include <QAction>
#include <QLabel>
#include <QStatusBar>
#include <QThread>
#include <common/CircularQueue.h>

#include "plugina.h"

PluginA::PluginA(QObject * parent)
    : QObject(parent)
{
}

QString PluginA::getname()
{
    return QString("插件A");
}

bool PluginA::init()
{
    return true;
}

bool PluginA::clean()
{
    return true;
}
