#include "client.h"
#include <QDebug>

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
    //UA_ReadRequest_clear(&req);
    //UA_ReadResponse_clear(&resp);
    //UA_NodeId_clear(&nodeId);

    return result;
}