
#include <QDebug>
#include <signal.h>
#include <stdlib.h>

extern "C"
{
#include <open62541/server_pubsub.h>
#include <open62541/server_config_default.h>
#include <open62541/types_generated.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/plugin/accesscontrol_default.h>
}

#include "opcuatranslate.h"
#include "opcuaserver.h"

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
}

OpcUaServer::~OpcUaServer()
{
    if (m_pServer)
    {
        UA_Server_delete(m_pServer);
    }
}

// 允许添加节点的访问控制回调函数
static UA_Boolean
allowAddNode(UA_Server * server, UA_AccessControl * ac, const UA_NodeId * sessionId, void * sessionContext, const UA_AddNodesItem * item)
{
    printf("Called allowAddNode\n");
    return UA_TRUE; // 允许添加节点
}

// 允许添加引用的访问控制回调函数
static UA_Boolean
allowAddReference(UA_Server * server, UA_AccessControl * ac, const UA_NodeId * sessionId, void * sessionContext, const UA_AddReferencesItem * item)
{
    printf("Called allowAddReference\n");
    return UA_TRUE; // 允许添加引用
}

// 禁止删除节点的访问控制回调函数
static UA_Boolean
allowDeleteNode(UA_Server * server, UA_AccessControl * ac, const UA_NodeId * sessionId, void * sessionContext, const UA_DeleteNodesItem * item)
{
    printf("Called allowDeleteNode\n");
    return UA_FALSE; // 禁止客户端删除节点
}

// 允许删除引用的访问控制回调函数
static UA_Boolean
allowDeleteReference(UA_Server * server, UA_AccessControl * ac, const UA_NodeId * sessionId, void * sessionContext, const UA_DeleteReferencesItem * item)
{
    printf("Called allowDeleteReference\n");
    return UA_TRUE; // 允许删除引用
}

// 设置自定义访问控制策略
static void
setCustomAccessControl(UA_ServerConfig * config, int maxConnect, UA_UsernamePasswordLogin userNamePW[1])
{
    if (maxConnect < 3)
        maxConnect = 3;
    // static UA_UsernamePasswordLogin userNamePW[1] = {
    //     {UA_STRING_STATIC("admin"), UA_STRING_STATIC("123456")}};
    //  使用默认访问控制插件作为基础
    UA_Boolean allowAnonymous   = false; // 禁止匿名登录
    UA_String  encryptionPolicy = config->securityPolicies[config->securityPoliciesSize - 1].policyUri;
    // UA_ByteString userTokenPolicyUri = UA_BYTESTRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#None");

    // 清除原有访问控制配置
    config->accessControl.clear(&config->accessControl);

    // 配置用户名密码认证方式（允许 peter 和 paula 登录）
    UA_AccessControl_default(config, allowAnonymous, &encryptionPolicy, maxConnect, userNamePW);

    // 自定义节点管理相关的权限控制
    config->accessControl.allowAddNode         = allowAddNode;
    config->accessControl.allowAddReference    = allowAddReference;
    config->accessControl.allowDeleteNode      = allowDeleteNode;
    config->accessControl.allowDeleteReference = allowDeleteReference;
}

bool OpcUaServer::startServer()
{
    // 设置服务器监听端口为 m_port，监听所有 IP（0.0.0.0），即 opc.tcp://<本机IP>:m_port
    UA_ServerConfig_setMinimal(m_pServerConfig, m_config.port, NULL);

    // 允许在无加密策略下使用用户名密码登录（仅用于测试环境）
    m_pServerConfig->allowNonePolicyPassword = true;

    QByteArray userNameArr = m_config.userName.toUtf8();
    QByteArray passwordArr = m_config.password.toUtf8();

    // 用户名密码登录列表
    UA_UsernamePasswordLogin logins[1];
    logins[0].username = UA_STRING_ALLOC(userNameArr.constData());
    logins[0].password = UA_STRING_ALLOC(passwordArr.constData());

    // 设置自定义访问控制策略
    setCustomAccessControl(m_pServerConfig, m_config.maxConnect, logins);

    // 添加变量节点（示例）
    if (!createNodes() || m_config.devices.isEmpty())
    {
        qDebug() << "Failed to create nodes";
        return false;
    }
    if (UA_Server_run_startup(m_pServer) != UA_STATUSCODE_GOOD)
        return false;

    m_pTimer->start(m_config.devices.first().period);
    return true;
    //  启动服务器主循环，直到 running 为 false
    // return (UA_Server_run(m_pServer, &g_running) == UA_STATUSCODE_GOOD); // 这里会阻塞，需要重构
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
        // qWarning() << "无该节点,无法更新节点数据:" << nodeBrowseName;
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
    else if (value.type() == QVariant::UInt)
    {
        int32_t val = value.toUInt();
        UA_Variant_setScalar(&var, &val, &UA_TYPES[UA_TYPES_UINT32]);
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
        const char *     textType       = "en";
        UA_LocalizedText deviceName     = UA_LOCALIZEDTEXT(const_cast<char *>(textType), const_cast<char *>(deviceNameUtf8.constData()));

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
            qWarning() << "创建父节点失败:" << device.device_name
                       << "状态码:" << UA_StatusCode_name(status);
            continue;
        }

        // 创建变量子节点
        for (VariableConfig & var : device.variables)
        {
            UA_NodeId varNodeId = createVariableNode(deviceNodeId, var, device.device_node_id);
            if (UA_NodeId_isNull(&varNodeId))
            {
                qWarning() << "创建子节点失败，节点可能已存在，请检查配置文件:" << var.browse_name;
                continue;
            }
            m_nodeMap.insert(var.browse_name, varNodeId);
            qDebug() << "创建子节点成功: " << device.device_node_id + "_" + var.browse_name;
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
    const char *     textType        = "en";
    UA_LocalizedText displayName     = UA_LOCALIZEDTEXT(const_cast<char *>(textType), const_cast<char *>(displayNameUtf8.constData()));
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
    else if (varConfig.data_type == "uint32")
    {
        attr.dataType = UA_TYPES[UA_TYPES_UINT32].typeId;
        uint32_t val  = varConfig.default_value.toUInt();
        UA_Variant_setScalar(&attr.value, &val, &UA_TYPES[UA_TYPES_UINT32]);
    }
    else if (varConfig.data_type == "int32")
    {
        attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
        int32_t val   = varConfig.default_value.toInt();
        UA_Variant_setScalar(&attr.value, &val, &UA_TYPES[UA_TYPES_INT32]);
    }
    else
    {
        qWarning() << "不支持的数据类型:" << varConfig.data_type;
        return UA_NODEID_NULL;
    }
    QString   nodeIdStr = deviceName + "_" + varConfig.browse_name;
    UA_NodeId nodeId    = createNodeIdFromQString(nodeIdStr);

    QByteArray       browseNameUtf8 = varConfig.browse_name.toUtf8();
    UA_QualifiedName browseName     = UA_QUALIFIEDNAME(1, const_cast<char *>(browseNameUtf8.constData()));

    UA_StatusCode status = UA_Server_addVariableNode(
        m_pServer,
        nodeId,       //  NodeId
        parentNodeId, // 传入的是已创建的真实父节点 ID
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        browseName,
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        attr,
        nullptr,
        &nodeId);

    // printNodeId(nodeId);

    if (status != UA_STATUSCODE_GOOD)
    {
        qWarning() << "添加节点失败:" << varConfig.browse_name
                   << "状态码:" << UA_StatusCode_name(status);
        return UA_NODEID_NULL;
    }

    return nodeId;
}

void OpcUaServer::setupPeriodicNodePublishing(const std::vector<UA_NodeId> & nodeList, double intervalMs, const QString & writerGroupName, const QString & dataSetWriterName)
{
    // 配置 PubSub 连接参数（使用 UDP 组播方式）
    UA_PubSubConnectionConfig connectionConfig;
    memset(&connectionConfig, 0, sizeof(connectionConfig));
    connectionConfig.name                = UA_STRING("UADP Connection"); // 连接名称
    connectionConfig.transportProfileUri = UA_STRING(
        "http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp"); // 使用 UDP+UADP 传输协议
    connectionConfig.enabled = UA_TRUE;                                   // 启用该连接

    // 设置 UDP 地址为组播地址（标准 OPC UA 示例地址）
    UA_NetworkAddressUrlDataType address;
    address.networkInterface = UA_STRING_NULL;                         // 默认网卡
    address.url              = UA_STRING("opc.udp://127.0.0.1:4840/"); // UDP 多播地址 // todo
    UA_Variant_setScalar(&connectionConfig.address, &address, &UA_TYPES[UA_TYPES_NETWORKADDRESSURLDATATYPE]);

    // 设置发布者ID（标识唯一发布者）
    connectionConfig.publisherIdType    = UA_PUBLISHERIDTYPE_UINT16;
    connectionConfig.publisherId.uint16 = 2234; // todo 可能改为可配置

    // 添加 PubSub 连接到服务器中
    UA_NodeId connectionId;
    if (UA_Server_addPubSubConnection(m_pServer, &connectionConfig, &connectionId) != UA_STATUSCODE_GOOD)
    {
        qWarning("添加 PubSub 连接失败");
        return;
    }

    // 创建 PublishedDataSet（发布的数据集）
    UA_PublishedDataSetConfig pdsConfig;
    memset(&pdsConfig, 0, sizeof(pdsConfig));
    pdsConfig.name                 = UA_STRING("MyDataSet");           // 数据集名称（内部可重复使用）
    pdsConfig.publishedDataSetType = UA_PUBSUB_DATASET_PUBLISHEDITEMS; // 按变量发布项

    UA_NodeId                    pdsId;
    UA_AddPublishedDataSetResult dataset = UA_Server_addPublishedDataSet(m_pServer, &pdsConfig, &pdsId);

    if (dataset.addResult != UA_STATUSCODE_GOOD)
    {
        qWarning("添加 PublishedDataSet 失败");
        return;
    }

    // 遍历节点列表，添加每个变量作为数据字段
    for (const UA_NodeId & nodeId : nodeList)
    {
        UA_DataSetFieldConfig fieldConfig;
        memset(&fieldConfig, 0, sizeof(fieldConfig));
        fieldConfig.dataSetFieldType                                   = UA_PUBSUB_DATASETFIELD_VARIABLE;
        fieldConfig.field.variable.publishParameters.publishedVariable = nodeId;               // 要发布的节点
        fieldConfig.field.variable.publishParameters.attributeId       = UA_ATTRIBUTEID_VALUE; // 发布 Value 属性
        fieldConfig.field.variable.promotedField                       = UA_TRUE;              // 作为独立数据字段传输

        UA_NodeId             fieldId;
        UA_DataSetFieldResult datasetfield = UA_Server_addDataSetField(m_pServer, pdsId, &fieldConfig, &fieldId);
        qDebug() << datasetfield.result;
        if (datasetfield.result != UA_STATUSCODE_GOOD)
        {
            qWarning("添加 DataSetField 失败");
        }
    }

    // 配置 WriterGroup（写组，控制发布节奏）
    UA_WriterGroupConfig wgConfig;
    memset(&wgConfig, 0, sizeof(wgConfig));
    wgConfig.publishingInterval = intervalMs; // 发布周期（毫秒）
    wgConfig.enabled            = UA_TRUE;
    wgConfig.writerGroupId      = 100;
    wgConfig.encodingMimeType   = UA_PUBSUB_ENCODING_UADP;

    // 设置 WriterGroup 的名称（从传入 QString 转为 UA_String）
    wgConfig.name = UA_STRING_ALLOC(writerGroupName.toUtf8().constData());

    // 配置 UADP 消息头格式（决定发布数据中包含哪些信息）
    UA_UadpWriterGroupMessageDataType * message = UA_UadpWriterGroupMessageDataType_new();
    message->networkMessageContentMask =
        UA_UADPNETWORKMESSAGECONTENTMASK_PUBLISHERID |
        UA_UADPNETWORKMESSAGECONTENTMASK_GROUPHEADER |
        UA_UADPNETWORKMESSAGECONTENTMASK_WRITERGROUPID |
        UA_UADPNETWORKMESSAGECONTENTMASK_PAYLOADHEADER;

    wgConfig.messageSettings.encoding             = UA_EXTENSIONOBJECT_DECODED;
    wgConfig.messageSettings.content.decoded.type = &UA_TYPES[UA_TYPES_UADPWRITERGROUPMESSAGEDATATYPE];
    wgConfig.messageSettings.content.decoded.data = message;

    // 添加 WriterGroup 到 PubSub 连接
    UA_NodeId writerGroupId;
    if (UA_Server_addWriterGroup(m_pServer, connectionId, &wgConfig, &writerGroupId) != UA_STATUSCODE_GOOD)
    {
        qWarning("添加 WriterGroup 失败");
        UA_UadpWriterGroupMessageDataType_delete(message);
        UA_String_clear(&wgConfig.name);
        return;
    }

    // 将 WriterGroup 设为运行状态
    UA_Server_setWriterGroupOperational(m_pServer, writerGroupId);

    // 释放动态分配的消息格式对象和名称字符串
    UA_UadpWriterGroupMessageDataType_delete(message);
    UA_String_clear(&wgConfig.name);

    // 配置 DataSetWriter（数据集写入器：负责具体将数据写入组播数据包）
    UA_DataSetWriterConfig dswConfig;
    memset(&dswConfig, 0, sizeof(dswConfig));
    dswConfig.dataSetWriterId = 1;
    dswConfig.keyFrameCount   = 10;
    dswConfig.name            = UA_STRING_ALLOC(dataSetWriterName.toUtf8().constData()); // 名称设置来自外部参数

    UA_NodeId dswId;

    if (UA_Server_addDataSetWriter(m_pServer, writerGroupId, pdsId, &dswConfig, &dswId) != UA_STATUSCODE_GOOD)
    {
        qWarning("添加 DataSetWriter 失败");
    }

    // 释放 DataSetWriter 名称的动态内存
    UA_String_clear(&dswConfig.name);

    qDebug("PubSub 发布设置完成：WriterGroup '%s', DataSetWriter '%s'",
           writerGroupName.toUtf8().constData(),
           dataSetWriterName.toUtf8().constData());
}

// 获取 NodeId 的字符串表示
std::string getNodeIdString(const UA_NodeId & nodeId)
{
    // 检查标识符类型是否为字符串类型
    if (nodeId.identifierType == UA_NODEIDTYPE_STRING)
    {
        // 如果是字符串类型，直接返回标识符
        UA_String nodeIdStr = nodeId.identifier.string;
        return std::string(reinterpret_cast<const char *>(nodeIdStr.data), nodeIdStr.length);
    }
    else if (nodeId.identifierType == UA_NODEIDTYPE_NUMERIC)
    {
        // 如果是数字类型，构建字符串表示
        return "ns=" + std::to_string(nodeId.namespaceIndex) + ";i=" + std::to_string(nodeId.identifier.numeric);
    }
    // else if (nodeId.identifierType == UA_NODEIDTYPE_GUID)
    //{
    //     // 如果是 GUID 类型，构建字符串表示
    //     char guidStr[64];
    //     snprintf(guidStr, sizeof(guidStr), "ns=%d;g=%s", nodeId.namespaceIndex, nodeId.identifier.guid);
    //     return std::string(guidStr);
    // }
    else
    {
        return "Unknown NodeId type";
    }
}
bool OpcUaServer::hasNode(const UA_NodeId & node)
{
    for (const UA_NodeId & nodeId : m_nodeMap)
    {
        if (getNodeIdString(nodeId) == getNodeIdString(node))
        {
            return true;
        }
    }

    return false;
}
