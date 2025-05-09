
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
UA_NodeId createNodeIdFromQString(const QString & str)
{
    QByteArray utf8 = str.toUtf8();
    return UA_NODEID_STRING_ALLOC(1, utf8.constData()); // utf8 生命周期在调用者手里
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

    static double qq = 0.1;
    //    Created variable node: "Current"
    // Created variable node: "Voltage"
    // Created variable node: "Power"
    // Created variable node: "SystemStatus"
    // Created variable node: "Speed"
    // Created variable node: "Heading
    if (qq == 0.1)
    {
        updateNodeData("Current", QVariant(11.0));
        updateNodeData("Voltage", QVariant(22.0));
        updateNodeData("Power", QVariant(33.0));
        updateNodeData("SystemStatus", QVariant(44));
        updateNodeData("Speed", QVariant(55.0 + qq));
        updateNodeData("Heading", QVariant(66.0));
    }
    qq += 0.1;
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
    //if (UA_Server_run_startup(m_pServer) != UA_STATUSCODE_GOOD)
    //    return false;

    //m_pTimer->start(m_config.devices.first().period);
    // 启动服务器主循环，直到 running 为 false
     return (UA_Server_run(m_pServer, &g_running) == UA_STATUSCODE_GOOD); // 这里会阻塞，需要重构
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

        QByteArray       deviceNameUtf8 = device.device_name.toUtf8();
        UA_LocalizedText deviceName     = UA_LOCALIZEDTEXT("en", const_cast<char *>(deviceNameUtf8.constData()));

        objAttr.displayName = deviceName;

        UA_NodeId     deviceNodeId;
        UA_StatusCode status = UA_Server_addObjectNode(
            m_pServer,
            createNodeIdFromQString(device.device_node_id),
            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
            UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
            UA_QUALIFIEDNAME(1, device.device_name.toUtf8().data()),
            UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
            objAttr,
            nullptr,
            &deviceNodeId);

        if (status != UA_STATUSCODE_GOOD)
        {
            qWarning() << "Failed to create device node:" << device.device_name
                       << "Status:" << UA_StatusCode_name(status);
            continue;
        }

        // 创建变量子节点
        for (VariableConfig & var : device.variables)
        {
            UA_NodeId varNodeId = createVariableNode(deviceNodeId, var, device.device_name); // ✅ 改为传 deviceNodeId
            if (UA_NodeId_isNull(&varNodeId))
            {
                qWarning() << "Failed to create variable node:" << var.browse_name;
                continue;
            }
            m_nodeMap.insert(var.browse_name, varNodeId);
            qDebug() << "Created variable node:" << var.browse_name;
            // break;
        }
    }
    return true;
}
void printNodeId(const UA_NodeId & nodeId)
{
    QString result = QString("ns=%1;").arg(nodeId.namespaceIndex);
    switch (nodeId.identifierType)
    {
    case UA_NODEIDTYPE_NUMERIC :
        result += QString("i=%1").arg(nodeId.identifier.numeric);
        break;
    case UA_NODEIDTYPE_STRING :
        result += QString("s=%1").arg(QString::fromUtf8((char *)nodeId.identifier.string.data,
                                                        static_cast<int>(nodeId.identifier.string.length)));
        break;
    case UA_NODEIDTYPE_GUID :
        result += "g=<GUID>"; // 可根据需要实现 GUID 打印
        break;
    case UA_NODEIDTYPE_BYTESTRING :
        result += "b=<ByteString>"; // 可根据需要实现 ByteString 打印
        break;
    default :
        result += "<Unknown>";
    }

    qDebug() << "NodeId:" << result;
}

UA_NodeId OpcUaServer::createVariableNode(UA_NodeId & parentNodeId, VariableConfig & varConfig, QString & deviceName)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;

    QByteArray       displayNameUtf8 = varConfig.display_name.toUtf8();
    UA_LocalizedText displayName     = UA_LOCALIZEDTEXT("en", const_cast<char *>(displayNameUtf8.constData()));
    attr.displayName                 = displayName;

    attr.accessLevel = (varConfig.access_level == "rw")
                           ? (UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE)
                           : UA_ACCESSLEVELMASK_READ;

    // 设置数据类型和初始值
    if (varConfig.data_type == "double")
    {
        attr.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;
        double val    = varConfig.default_value.toDouble();
        UA_Variant_setScalar(&attr.value, &val, &UA_TYPES[UA_TYPES_DOUBLE]);
    }
    else if (varConfig.data_type == "int32")
    {
        attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
        int32_t val   = varConfig.default_value.toInt();
        UA_Variant_setScalar(&attr.value, &val, &UA_TYPES[UA_TYPES_INT32]);
    }
    else
    {
        qWarning() << "Unsupported data type:" << varConfig.data_type;
        return UA_NODEID_NULL;
    }
    QString   nodeIdStr = deviceName + "_" + varConfig.browse_name;
    UA_NodeId nodeId    = createNodeIdFromQString(nodeIdStr); // 使用稳定数据

    QByteArray       browseNameUtf8 = varConfig.browse_name.toUtf8();
    UA_QualifiedName browseName     = UA_QUALIFIEDNAME(1, const_cast<char *>(browseNameUtf8.constData()));

    UA_StatusCode status = UA_Server_addVariableNode(
        m_pServer,
        nodeId,       //  NodeId
        parentNodeId, // ✅ 传入的是已创建的真实父节点 ID
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        browseName,
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        attr,
        nullptr,
        &nodeId);

    printNodeId(nodeId);

    if (status != UA_STATUSCODE_GOOD)
    {
        qWarning() << "Failed to add variable node:" << varConfig.browse_name
                   << "Status:" << UA_StatusCode_name(status);
        return UA_NODEID_NULL;
    }

    return nodeId;
}
