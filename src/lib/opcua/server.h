#ifndef SERVER_H
#define SERVER_H

#include <open62541/server.h>
#include "config.h"

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QMap>

/// <summary>
/// �����ʾһ���˿ڵķ��񣬸ö˿ڵķ���˿�һ�£����ݷ��ͼ��һ�£������ж���ڵ�
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