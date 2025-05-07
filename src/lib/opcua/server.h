#ifndef SERVER_H
#define SERVER_H

#include <open62541/server.h>
#include "config.h"

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QMap>
#include <QVector>

/// <summary>
/// 该类表示一个端口的服务，该端口的服务端口一致，数据发送间隔一致，可以有多个节点
/// </summary>
class OpcUaServer : public QObject
{
    Q_OBJECT
public:
    OpcUaServer(quint16 port, int period, QObject * parent = nullptr);
    ~OpcUaServer() override;

    void startServer();
    void stopServer();

    // 添加节点,当前服务下有哪些节点
    void addNode(UA_NodeId node);

    // 更新节点数据
    void updateNodeData(const QString & nodeBrowseName, const QVariant & value);
   
    // 根据配置创建节点
    bool createNodes();

private:
    double  m_period; // 数据发送间隔，单位为毫秒
    quint16 m_port;   // 端口

    QTimer *          m_pTimer; // 定时器
    UA_Server *       m_server; // opcua服务器
    UA_ServerConfig * m_config; // opcua配置

    QMap<UA_NodeId, QVector<UA_Variant>> m_nodeMap; // 一个节点下的所有参数(具体的数据值)

    QVector<UA_NodeId> m_nodeIds; // 节点ID列表
};

#endif // SERVER_H