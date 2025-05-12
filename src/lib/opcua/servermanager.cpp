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

void ServerManager::setupPeriodicNodePublishing(const std::vector<UA_NodeId> & nodeList, double intervalMs, const QString & writerGroupName, const QString & dataSetWriterName)
{
    for (auto node : nodeList)
    {
        for (auto server : m_servers)
        {
            if (server->hasNode(node))
            {
                // todo 这里后面肯需要重新处理
                server->setupPeriodicNodePublishing(nodeList, intervalMs, writerGroupName, dataSetWriterName);
                return;
            }
        }
    }
}
