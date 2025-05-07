#ifndef SERVER_H
#define SERVER_H

#include <open62541/server.h>
#include "config.h"

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QMap>

/// <summary>
/// 该类表示一个端口的服务，该端口的服务端口一致，数据发送间隔一致，可以有多个节点
/// </summary>
class OpcUaServer : public QObject
{
    Q_OBJECT
public:
    OpcUaServer(QObject * parent = nullptr);
    ~OpcUaServer() override;

    void startServer();
    void stopServer();

private:
    quint16     m_port;
    UA_Server * m_server;
    UA_Boolean  m_running;

    QMap<UA_NodeId, > m_nodeMap;
};

#endif // SERVER_H