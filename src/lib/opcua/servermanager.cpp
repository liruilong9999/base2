#include "servermanager.h"

ServerManager::ServerManager(QObject * parent)
    : QObject(parent)
{
}

ServerManager::~ServerManager()
{
}

void ServerManager::loadConfig(QString path)
{
    m_config = ConfigParser::parse(path);

    if (m_config.isEmpty())
        return;



}

void ServerManager::startThread()
{
}
