#include "opcuaservermanager.h"

OpcUaServerManager::OpcUaServerManager(QObject * parent)
    : QObject(parent)
{
}

OpcUaServerManager::~OpcUaServerManager()
{
}

void OpcUaServerManager::loadConfig(QString path)
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

void OpcUaServerManager::startThread()
{
    for (auto server : m_servers)
    {
        server->startServer();
    }
}

void OpcUaServerManager::setupPeriodicNodePublishing(const std::vector<UA_NodeId> & nodeList, double intervalMs, const QString & writerGroupName, const QString & dataSetWriterName)
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
