
#include <QDebug>
#include <open62541/server_config_default.h>
#include <signal.h>
#include <stdlib.h>

#include "translate.h"
#include "server.h"

// 控制服务器运行状态的标志
UA_Boolean g_running = true;

// Ctrl+C 信号处理函数
static void stopHandler(int sign)
{
    // UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "接收到 Ctrl+C 中断信号，准备停止服务器");
    g_running = false;
}

OpcUaServer::OpcUaServer(quint16 port, int period, QObject * parent)
    : QObject(parent)
    , m_period(period)
    , m_port(port)
    , m_pTimer(new QTimer(this))
{
    // 注册信号处理器，接收 Ctrl+C 中断信号
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    // 创建服务器实例
    m_server = UA_Server_new();
    m_config = UA_Server_getConfig(m_server);
}

OpcUaServer::~OpcUaServer()
{
    if (m_server)
    {
        UA_Server_delete(m_server);
    }
}

void OpcUaServer::startServer()
{
    // 设置服务器监听端口为 m_port，监听所有 IP（0.0.0.0），即 opc.tcp://<本机IP>:m_port
    UA_ServerConfig_setMinimal(m_config, m_port, NULL);

    // 添加变量节点（示例）

    // 启动服务器主循环，直到 running 为 false
    UA_StatusCode retval = UA_Server_run(m_server, &g_running); // 这里会阻塞，需要重构
}

void OpcUaServer::stopServer()
{
    g_running = false;
}

void OpcUaServer::addNode(UA_NodeId node)
{
    m_nodeIds.push_back(node);
}

void OpcUaServer::updateNodeData(const QString & nodeBrowseName, const QVariant & value)
{
}

bool OpcUaServer::createNodes()
{
    return false;
}
