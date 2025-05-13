#include <QDebug>

#include "opcuaclientmanager.h"

OpcUaClientManager::OpcUaClientManager(QObject * parent)
    : QThread(parent)
{
    // qDebug() << "__________OpcUaClientManager thread______" << QThread::currentThreadId();
}

OpcUaClientManager::~OpcUaClientManager()
{
    quit();
    wait();
}

void OpcUaClientManager::loadConfig(QString path)
{
    QMutexLocker locker(&m_mutex);
    bool         isSuccess = ConfigParser::parseConfig(path, m_config);

    if (!isSuccess || m_config.isEmpty())
        return;

    for (OpcUaConfig serverConfig : m_config)
    {
        OpcUaClientSync * clientSync = new OpcUaClientSync(serverConfig, serverConfig.url, this);
        m_syncClients.insert(serverConfig.url, clientSync);
    }
}

void OpcUaClientManager::startThread()
{
    for (auto client : m_syncClients)
    {
        client->connectToServer();
    }
    start();
}

std::pair<bool, QVariant> OpcUaClientManager::getNodeData(const QString & parentNode, const QString & subNode)
{
    if (m_syncClients.size() <= 0)
        return std::pair<bool, QVariant>(false, QVariant());

    std::pair<bool, QVariant> resData;
    for (auto client : m_syncClients)
    {
        bool hasNode = client->hasNode(parentNode, subNode);
        if (hasNode == true)
        {
            return std::pair<bool, QVariant>(true, client->readValue(parentNode, subNode));
        }
    }
    return std::pair<bool, QVariant>(false, QVariant());
}

bool OpcUaClientManager::writeValue(const QString & parentNode, const QString & subNode, QVariant & value)
{
    if (m_syncClients.size() <= 0)
        return false;

    for (auto client : m_syncClients)
    {
        QString nodeStr = parentNode + "_" + subNode;
        bool    res     = client->canWriteValue(parentNode, subNode, value);
        if (res)
        {
            return true;
        }
    }
    return false;
}

bool OpcUaClientManager::subscribeNodeValueSlot(const QString & parentNodeId, const QString & subNodeId)
{
    if (m_asynClients.size() <= 0)
        return false;

    for (auto client : m_asynClients)
    {
        bool res = client->hasNode(parentNodeId, subNodeId);
        if (res)
        {
            client->subscribeNodeValue(parentNodeId, subNodeId);
            return true;
        }
    }
    return false;
}

void OpcUaClientManager::run()
{
    // qDebug() << "__________run thread______" << QThread::currentThreadId();
    QMutexLocker locker(&m_mutex);
    for (OpcUaConfig serverConfig : m_config)
    {
        OpcUaClientAsyn * clientAsyn = new OpcUaClientAsyn(serverConfig, serverConfig.url);
        m_asynClients.insert(serverConfig.url, clientAsyn);
    }
    for (auto client : m_asynClients)
    {
        client->connectToServer();
    }
    connect(this, &OpcUaClientManager::subscribeNodeValue, this, &OpcUaClientManager::subscribeNodeValueSlot, Qt::QueuedConnection);

    exec();

    for (auto & client : m_asynClients)
    {
        delete client;
        client = nullptr;
    }
}
