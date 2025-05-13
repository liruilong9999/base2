#ifndef SERVER_H
#define SERVER_H

#include <open62541/server.h>
#include "opcuaconfig.h"

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QMap>
#include <QVector>
#include <QVariant>
#include "opcua_global.h"

/// <summary>
/// 该类表示一个端口的服务，该端口的服务端口一致，数据发送间隔一致(多个设备数据发送间隔依旧一致)，可以有多个节点
/// </summary>
class OPCUA_EXPORT OpcUaServer : public QObject
{
    Q_OBJECT
public:
    OpcUaServer(OpcUaConfig config, QObject * parent = nullptr);
    ~OpcUaServer() override;

    bool startServer();
    void stopServer();

    // 添加节点,当前服务下有哪些节点
    void addNode(UA_NodeId node);

    // 根据配置创建节点
    bool createNodes();

    // 创建单个变量节点
    UA_NodeId createVariableNode(UA_NodeId & parentNodeIdStr, VariableConfig & varConfig, QString & deviceName);

    void setupPeriodicNodePublishing(const std::vector<UA_NodeId> & nodeList,
                                     double                         intervalMs,
                                     const QString &                writerGroupName,
                                     const QString &                dataSetWriterName);
    bool hasNode(const UA_NodeId & node);
public slots:
    // 周期性更新数据
    void onTimerTimeout();
    // 更新节点数据
    void updateNodeData(const QString & nodeBrowseName, const QVariant & value); // todo ,需要一个地方调用更新，跨线程

private:
    QTimer *          m_pTimer;        // 定时器
    UA_Server *       m_pServer;       // opcua服务器
    UA_ServerConfig * m_pServerConfig; // opcua配置

    // 节点映射表 <BrowseName, NodeId>
    QMap<QString, UA_NodeId> m_nodeMap; // 具体的某个子节点名称和子节点nodeId

    QVector<UA_NodeId> m_nodeIds; // 节点ID列表
    OpcUaConfig        m_config;  // 配置文件参数
};

#endif // SERVER_H