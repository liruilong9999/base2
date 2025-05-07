#include "server.h"
#include <QDebug>
#include <open62541/server_config_default.h>
#include "translate.h"

// 写入回调函数的上下文数据
struct WriteContext
{
    OpcUaServer * server;
    NodeConfig    nodeCfg;
};

// 静态写入回调函数
static UA_StatusCode writeCallback(UA_Server *             server,
                                   const UA_NodeId *       sessionId,
                                   void *                  sessionContext,
                                   const UA_NodeId *       nodeId,
                                   void *                  nodeContext,
                                   const UA_NumericRange * range,
                                   const UA_DataValue *    value)
{
    Q_UNUSED(sessionId);
    Q_UNUSED(sessionContext);
    Q_UNUSED(range);
    WriteContext * ctx = static_cast<WriteContext *>(nodeContext);
    qDebug() << "节点" << ctx->nodeCfg.nodeId << "被写入新值";
    

        // 添加值范围验证等逻辑
    if (value->hasValue && value->value.type == &UA_TYPES[UA_TYPES_INT32])
    {
        int32_t newValue = *(int32_t *)value->value.data;
        if (newValue < ctx->nodeCfg.minValue || newValue > ctx->nodeCfg.maxValue)
        {
            return UA_STATUSCODE_BADOUTOFRANGE;
        }
    }

    return UA_STATUSCODE_GOOD;
}

OpcUaServer::OpcUaServer()
    : m_server(nullptr)
    , m_running(false)
{}

OpcUaServer::~OpcUaServer()
{
    stop();
}

bool OpcUaServer::start(QVector<OpcUaConfig> & configs)
{
    m_server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(m_server));

    // 遍历所有配置创建节点
    for (OpcUaConfig & config : configs)
    {
        createNodes(config);
    }

    m_running         = true;
    UA_StatusCode ret = UA_Server_run(m_server, &m_running);
    if (ret != UA_STATUSCODE_GOOD)
    {
        qCritical() << "服务器启动失败，错误码:" << UA_StatusCode_name(ret);
        return false;
    }
    return true;
}

void OpcUaServer::stop()
{
    m_running = false;
    if (m_server)
    {
        UA_Server_delete(m_server);
        m_server = nullptr;
    }
}

void OpcUaServer::createNodes(OpcUaConfig & config)
{
    UA_NodeId           deviceFolder;
    UA_ObjectAttributes objAttr = UA_ObjectAttributes_default;

    objAttr.displayName = UA_LOCALIZEDTEXT("en", QString2Char(config.device));
    UA_Server_addObjectNode(m_server, UA_NODEID_NUMERIC(1, 0), UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER), UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, QString2Char(config.device)), UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE), objAttr, nullptr, &deviceFolder);

    for (NodeConfig & nodeCfg : config.data)
    {
        UA_VariableAttributes varAttr = UA_VariableAttributes_default;
        varAttr.displayName           = UA_LOCALIZEDTEXT("en", QString2Char(nodeCfg.displayName));
        varAttr.dataType              = mapDataType(nodeCfg.dataType)->typeId;
        varAttr.accessLevel           = mapAccessLevel(nodeCfg.accessLevel);

        // 设置默认值
        if (nodeCfg.dataType == "int32")
        {
            int32_t val = nodeCfg.defaultValue.toInt();
            UA_Variant_setScalar(&varAttr.value, &val, &UA_TYPES[UA_TYPES_INT32]);
        } // 其他数据类型处理...

        // 创建变量节点
        UA_NodeId        nodeId     = UA_NODEID_STRING(1, QString2Char(nodeCfg.nodeId));
        UA_QualifiedName browseName = UA_QUALIFIEDNAME(1, QString2Char(nodeCfg.browseName));
        UA_Server_addVariableNode(m_server, nodeId, deviceFolder, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT), browseName, UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), varAttr, nullptr, nullptr);

         // 注册写入回调（最新版本方式）
        if (nodeCfg.accessLevel.contains("w"))
        {
            WriteContext *   ctx = new WriteContext{this, nodeCfg};
            UA_ValueCallback callback;
            callback.onWrite      = writeCallback;
            callback.onRead       = nullptr;
            UA_Server_setVariableNode_valueCallback(m_server, nodeId, callback, ctx);
        }
    }
}

const UA_DataType * OpcUaServer::mapDataType(QString & typeStr)
{
    static QMap<QString, const UA_DataType *> typeMap = {
        {"int32", &UA_TYPES[UA_TYPES_INT32]},
        {"double", &UA_TYPES[UA_TYPES_DOUBLE]},
        {"bool", &UA_TYPES[UA_TYPES_BOOLEAN]},
        // 其他类型...
    };
    return typeMap.value(typeStr, &UA_TYPES[UA_TYPES_VARIANT]);
}

UA_Byte OpcUaServer::mapAccessLevel(QString & accessLevel)
{
    if (accessLevel == "rw")
        return UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    return UA_ACCESSLEVELMASK_READ;
}