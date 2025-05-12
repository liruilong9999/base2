#include "client.h"
#include <QDebug>

extern "C"
{
#include <open62541/client_config_default.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>
}

OpcUaClient::OpcUaClient(QObject * parent)
    : QObject(parent)
{
    m_client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(m_client));
}

OpcUaClient::~OpcUaClient()
{
    if (m_client)
    {
        UA_Client_disconnect(m_client);
        UA_Client_delete(m_client);
    }
}

bool OpcUaClient::connectToServer(const QString & url)
{
    UA_StatusCode status = UA_Client_connect(m_client, url.toUtf8().constData());
    if (status != UA_STATUSCODE_GOOD)
    {
        qWarning() << "Failed to connect to server:" << UA_StatusCode_name(status);
        return false;
    }
    return true;
}

void OpcUaClient::disconnectFromServer()
{
    if (m_client)
    {
        UA_Client_disconnect(m_client);
    }
}

#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/plugin/log_stdout.h>
#include <QString>
#include <QDebug>

// 回调函数：当订阅节点的值发生变化时会调用此函数
static void handler_DataChanged(UA_Client * client, UA_UInt32 subId, void * subContext, UA_UInt32 monId, void * monContext, UA_DataValue * value)
{
    UA_Variant * variant = &value->value;
    if (UA_Variant_hasScalarType(variant, &UA_TYPES[UA_TYPES_DOUBLE]))
    {
        double v = *(UA_Double *)variant->data;
        qDebug() << "值变更：" << v;
    }
    else
    {
        qDebug() << "接收到其他类型的数据";
    }
}

// 客户端订阅函数
void OpcUaClient::subscribeNodeValue(const QString & nodeIdStr)
{
    // 创建并配置默认客户端
    UA_Client * client = m_client;

    // 解析 nodeId 字符串为 UA_NodeId
    UA_NodeId nodeId = UA_NODEID_STRING_ALLOC(1, nodeIdStr.toUtf8().constData());

    // 创建订阅（发布间隔 1000ms）
    UA_CreateSubscriptionRequest request   = UA_CreateSubscriptionRequest_default();
    request.requestedPublishingInterval    = 1000.0;
    UA_CreateSubscriptionResponse response = UA_Client_Subscriptions_create(client, request, NULL, NULL, NULL);
    if (response.responseHeader.serviceResult != UA_STATUSCODE_GOOD)
    {
        qWarning("创建订阅失败");
        UA_NodeId_clear(&nodeId);
        UA_Client_disconnect(client);
        UA_Client_delete(client);
        return;
    }

    UA_UInt32 subId = response.subscriptionId;
    qDebug("成功创建订阅，ID = %u", subId);

    // 设置监控项，订阅该节点的值变化
    UA_MonitoredItemCreateRequest monRequest =
        UA_MonitoredItemCreateRequest_default(nodeId);
    monRequest.requestedParameters.samplingInterval = 500.0; // 采样间隔

    UA_MonitoredItemCreateResult monResponse =
        UA_Client_MonitoredItems_createDataChange(client, subId, UA_TIMESTAMPSTORETURN_SOURCE, monRequest, NULL, handler_DataChanged, NULL);
    if (monResponse.statusCode != UA_STATUSCODE_GOOD)
    {
        qWarning("创建监控项失败");
        UA_NodeId_clear(&nodeId);
        UA_Client_disconnect(client);
        UA_Client_delete(client);
        return;
    }

    UA_NodeId_clear(&nodeId);
}

QVariant OpcUaClient::readValue(const QString & fullNodeId)
{
    // 1. 转换节点ID
    UA_NodeId nodeId = UA_NODEID_STRING_ALLOC(1, "Current");

    // 2. 初始化读取请求
    UA_ReadRequest req;
    UA_ReadRequest_init(&req);
    req.nodesToRead     = UA_ReadValueId_new();
    req.nodesToReadSize = 1;

    // 3. 配置要读取的节点属性
    req.nodesToRead[0].nodeId       = nodeId;
    req.nodesToRead[0].attributeId  = UA_ATTRIBUTEID_VALUE;
    req.nodesToRead[0].indexRange   = UA_STRING_NULL;                       // 必须初始化
    req.nodesToRead[0].dataEncoding = UA_QUALIFIEDNAME(0, "DefaultBinary"); // 编码方式

    // 4. 执行读取
    UA_ReadResponse resp   = UA_Client_Service_read(m_client, req);
    UA_StatusCode   retval = resp.responseHeader.serviceResult;

    QVariant result;
    if (retval == UA_STATUSCODE_GOOD && resp.resultsSize > 0)
    {
        UA_DataValue * dv = &resp.results[0];
        if (dv->hasValue && UA_Variant_hasScalarType(&dv->value, &UA_TYPES[UA_TYPES_DOUBLE]))
        {
            result = *static_cast<double *>(dv->value.data);
        }
    }
    else
    {
        qWarning() << "Read failed. Status:" << UA_StatusCode_name(retval)
                   << "| Node:" << fullNodeId;
    }

    // 5. 清理资源
    // UA_ReadRequest_clear(&req);
    // UA_ReadResponse_clear(&resp);
    // UA_NodeId_clear(&nodeId);

    return result;
}