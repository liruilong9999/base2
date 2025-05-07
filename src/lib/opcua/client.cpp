#include "client.h"
#include <QDebug>

#include <open62541/client_config_default.h>

OpcUaClient::OpcUaClient(QObject * parent)
    : QObject(parent)
    , m_client(UA_Client_new())
{
    UA_ClientConfig_setDefault(UA_Client_getConfig(m_client));
}

OpcUaClient::~OpcUaClient()
{
    disconnect();
    UA_Client_delete(m_client);
}

bool OpcUaClient::connect(const QString & url)
{
    UA_StatusCode status = UA_Client_connect(m_client, url.toUtf8());
    if (status != UA_STATUSCODE_GOOD)
    {
        qWarning() << "连接失败:" << UA_StatusCode_name(status);
        return false;
    }
    return true;
}

void OpcUaClient::disconnect()
{
    UA_Client_disconnect(m_client);
}

bool OpcUaClient::writeValue(const QString & nodeId, const QVariant & value)
{
    UA_NodeId  uaNodeId = UA_NODEID_STRING(1, nodeId.toUtf8());
    UA_Variant uaValue;
    UA_Variant_init(&uaValue);

    // 根据类型设置值
    switch (value.type())
    {
    case QVariant::Int :
        UA_Variant_setScalar(&uaValue, (int32_t *)&value.toInt(), &UA_TYPES[UA_TYPES_INT32]);
        break;
    // 其他类型处理...
    default :
        qWarning() << "不支持的数据类型";
        return false;
    }

    UA_StatusCode status = UA_Client_writeValueAttribute(m_client, uaNodeId, &uaValue);
    return (status == UA_STATUSCODE_GOOD);
}

void OpcUaClient::subscribe(const QString & nodeId, int intervalMs)
{
    // 创建订阅
    UA_CreateSubscriptionRequest subReq   = UA_CreateSubscriptionRequest_default();
    subReq.requestedPublishingInterval    = intervalMs;
    UA_CreateSubscriptionResponse subResp = UA_Client_Subscriptions_create(m_client, subReq, nullptr, nullptr, nullptr);

    // 创建监控项
    UA_MonitoredItemCreateRequest itemReq;
    UA_MonitoredItemCreateRequest_init(&itemReq);
    itemReq.itemToMonitor.nodeId      = UA_NODEID_STRING(1, nodeId.toUtf8());
    itemReq.itemToMonitor.attributeId = UA_ATTRIBUTEID_VALUE;
    itemReq.monitoringMode            = UA_MONITORINGMODE_REPORTING;

    UA_Client_MonitoredItems_createDataChange(
        m_client, subResp.subscriptionId, UA_TIMESTAMPSTORETURN_BOTH, itemReq, nullptr, [](UA_Client * client, UA_UInt32 subId, void * subContext, UA_UInt32 monId, void * monContext, UA_DataValue * value) {
            // 触发信号
            QVariant var;
            if (value->hasValue && value->value.type == &UA_TYPES[UA_TYPES_INT32])
            {
                var = QVariant(*(int32_t *)value->value.data);
            }
            OpcUaClient * clientObj = static_cast<OpcUaClient *>(monContext);
            emit          clientObj->valueChanged(QString::fromUtf8(UA_String_getChars(&value->value.type->typeName)), var);
        },
        this);
}