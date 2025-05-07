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
    bool isSuccess = ConfigParser::parseConfig(path, m_config);

    if (!isSuccess || m_config.isEmpty())
        return;

    for (OpcUaConfig serverConfig : m_config)
    {
        OpcUaServer * server = new OpcUaServer(serverConfig, this);
        m_servers.insert(serverConfig.url, server);
    }
}

void ServerManager::startThread()
{
    for (auto server : m_servers)
    {
        server->startServer();
    }
}
