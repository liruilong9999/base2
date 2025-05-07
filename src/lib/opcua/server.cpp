
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

OpcUaServer::OpcUaServer(OpcUaConfig config, QObject * parent)
    : QObject(parent)
    , m_config(config)
    , m_pTimer(new QTimer(this))
{
    // 注册信号处理器，接收 Ctrl+C 中断信号
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    // 创建服务器实例
    m_pServer       = UA_Server_new();
    m_pServerConfig = UA_Server_getConfig(m_pServer);

    connect(m_pTimer, &QTimer::timeout, this, &OpcUaServer::onTimerTimeout);
}

void OpcUaServer::onTimerTimeout()
{
    UA_Server_run_iterate(m_pServer, false); // 非阻塞模式;
    // updateNodeData(const QString & nodeBrowseName, const QVariant & value);
}

OpcUaServer::~OpcUaServer()
{
    if (m_pServer)
    {
        UA_Server_delete(m_pServer);
    }
}

bool OpcUaServer::startServer()
{
    // 设置服务器监听端口为 m_port，监听所有 IP（0.0.0.0），即 opc.tcp://<本机IP>:m_port
    UA_ServerConfig_setMinimal(m_pServerConfig, m_config.port, NULL);

    // 设置服务器URL
    // UA_String hostname = UA_STRING_ALLOC(m_config.url.toUtf8().constData());
    // UA_ServerConfig_sethostname(UA_Server_getConfig(m_pServer), hostname);
    // UA_String_clear(&hostname);

    // 添加变量节点（示例）
    if (!createNodes() || m_config.devices.isEmpty())
    {
        qDebug() << "Failed to create nodes";
        return false;
    }
    m_pTimer->start(m_config.devices.first().period);
    // 启动服务器主循环，直到 running 为 false
    // return (UA_Server_run(m_pServer, &g_running) == UA_STATUSCODE_GOOD;; // 这里会阻塞，需要重构
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
    if (!m_nodeMap.contains(nodeBrowseName))
    {
        qWarning() << "Unknown node:" << nodeBrowseName;
        return;
    }

    UA_NodeId  nodeId = m_nodeMap[nodeBrowseName];
    UA_Variant var;
    UA_Variant_init(&var);

    if (value.type() == QVariant::Double)
    {
        double val = value.toDouble();
        UA_Variant_setScalar(&var, &val, &UA_TYPES[UA_TYPES_DOUBLE]);
    }
    else if (value.type() == QVariant::Int)
    {
        int32_t val = value.toInt();
        UA_Variant_setScalar(&var, &val, &UA_TYPES[UA_TYPES_INT32]);
    }
    // 其他类型处理...

    UA_Server_writeValue(m_pServer, nodeId, var);
}

bool OpcUaServer::createNodes()
{
    for (DeviceConfig & device : m_config.devices)
    {
        // 创建设备父节点
        UA_ObjectAttributes objAttr = UA_ObjectAttributes_default;
        objAttr.displayName         = UA_LOCALIZEDTEXT("en", device.device_name.toUtf8().data());

        UA_NodeId deviceNodeId;
        UA_Server_addObjectNode(m_pServer,
                                UA_NODEID_STRING(1, device.device_node_id.toUtf8().data()),
                                UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                                UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                UA_QUALIFIEDNAME(1, device.device_name.toUtf8().data()),
                                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
                                objAttr,
                                nullptr,
                                &deviceNodeId);

        // 创建变量子节点
        for (VariableConfig & var : device.variables)
        {
            UA_NodeId varNodeId = createVariableNode(device.device_node_id, var);
            if (UA_NodeId_isNull(&varNodeId))
            {
                qWarning() << "Failed to create node:" << var.browse_name;
                continue;
            }
            m_nodeMap.insert(var.browse_name, varNodeId);
        }
    }
    return true;
}

UA_NodeId OpcUaServer::createVariableNode(QString &parentNodeIdStr, VariableConfig &varConfig)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en", varConfig.display_name.toUtf8().data());
    attr.accessLevel = varConfig.access_level == "rw"
        ? (UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE)
        : UA_ACCESSLEVELMASK_READ;

    // 设置数据类型和初始值
    if (varConfig.data_type == "double") {
        attr.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;
        double val = varConfig.default_value.toDouble();
        UA_Variant_setScalar(&attr.value, &val, &UA_TYPES[UA_TYPES_DOUBLE]);
    } else if (varConfig.data_type == "int32") {
        attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
        int32_t val = varConfig.default_value.toInt();
        UA_Variant_setScalar(&attr.value, &val, &UA_TYPES[UA_TYPES_INT32]);
    }

    UA_NodeId nodeId = UA_NODEID_NULL;
    UA_QualifiedName browseName = UA_QUALIFIEDNAME(1, varConfig.browse_name.toUtf8().data());
    UA_NodeId parentNodeId = UA_NODEID_STRING(1, parentNodeIdStr.toUtf8().data());

    UA_StatusCode status = UA_Server_addVariableNode(
        m_pServer, UA_NODEID_NULL, parentNodeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        browseName, UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        attr, nullptr, &nodeId);

    if (status != UA_STATUSCODE_GOOD) {
        qWarning() << "Failed to add variable node:" << varConfig.browse_name
                   << "Status:" << UA_StatusCode_name(status);
        return UA_NODEID_NULL;
    }

    return nodeId;
}
