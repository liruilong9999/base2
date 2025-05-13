#include <QDebug>
#include <QTimer>

extern "C"
{
#include <open62541/client_config_default.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>
}
#include "opcuaclientsync.h"

OpcUaClientSync::OpcUaClientSync(OpcUaConfig config, QString url, QObject * parent)
    : QObject(parent)
    , m_config(config)
    , m_url(url)
{
    m_client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(m_client));

    // 添加定时器，每 100ms 调用一次 UA_Client_run_iterate() 处理订阅消息
    QTimer * timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (m_client)
        {
            UA_Client_run_iterate(m_client, false); // 必须调用！
        }
    });
    timer->start(100); // 调用频率应不低于服务端的发布频率
}

OpcUaClientSync::~OpcUaClientSync()
{
    if (m_client)
    {
        UA_Client_disconnect(m_client);
        UA_Client_delete(m_client);
    }
}

bool OpcUaClientSync::connectToServer()
{
    QByteArray urlUtf8      = m_url.toUtf8();
    QByteArray userNameUtf8 = m_config.userName.toUtf8();
    QByteArray passwordUtf8 = m_config.password.toUtf8();

    // UA_StatusCode status = UA_Client_connect(m_client, const_cast<char *>(urlUtf8.constData()));
    UA_StatusCode status = UA_Client_connectUsername(
        m_client,
        const_cast<char *>(urlUtf8.constData()),
        const_cast<char *>(userNameUtf8.constData()), // 用户名（与服务端设置一致）
        const_cast<char *>(passwordUtf8.constData())  // 密码（与服务端设置一致）
    );
    if (status != UA_STATUSCODE_GOOD)
    {
        qWarning() << "连接服务端失败,状态为:<" << UA_StatusCode_name(status) << "> 服务端url为:" << m_url;
        return false;
    }
    return true;
}

QVariant OpcUaClientSync::readValue(const QString & parentNodeId, const QString & subNodeId)
{
    const QString  fullNodeId = parentNodeId + "_" + subNodeId;
    UA_ReadValueId itemArray[1]; // 存储需要读取的节点信息

    // 初始化要读取的节点

    UA_ReadValueId_init(&itemArray[0]);
    itemArray[0].attributeId = UA_ATTRIBUTEID_VALUE; // 指定读取的是“值”属性

    // 设置各节点的 NodeId

    QByteArray nodeNameUtf8 = fullNodeId.toUtf8();
    itemArray[0].nodeId     = UA_NODEID_STRING(1, const_cast<char *>(nodeNameUtf8.constData()));

    // 创建读取请求
    UA_ReadRequest request;
    UA_ReadRequest_init(&request);
    request.nodesToRead     = &itemArray[0];
    request.nodesToReadSize = 1;

    // 发送读取请求
    UA_ReadResponse response = UA_Client_Service_read(m_client, request);

    UA_StatusCode retStatusArray[1];
    UA_StatusCode retval = response.responseHeader.serviceResult;

    // 判断整体请求是否成功
    if (retval == UA_STATUSCODE_GOOD)
    {
        // 检查结果数量是否与请求数量一致
        if (response.resultsSize == 1)
        {
            retStatusArray[0] = response.results[0].status; // 记录每个节点的读取状态码
        }
        else
        {
            UA_ReadResponse_clear(&response);
            return QVariant(); // 结果数量不对
        }
    }

    // 读取结果
    if (retStatusArray[0] == UA_STATUSCODE_GOOD)
    {
        UA_DataValue res = response.results[0];

        if (!res.hasValue) // 没有返回值
        {
            UA_ReadResponse_clear(&response);
            return QVariant();
        }

        UA_Variant out;
        memcpy(&out, &res.value, sizeof(UA_Variant)); // 拷贝 Variant
        UA_Variant_init(&res.value);                  // 清空原值，避免重复释放

        // 判断数据类型
        if (out.type == &UA_TYPES[UA_TYPES_LOCALIZEDTEXT])
        {
            UA_LocalizedText * ptr = (UA_LocalizedText *)out.data;
            // printf("Text: %.*s\n", ptr->text.length, ptr->text.data);
            QString resStr = QString::fromUtf8(reinterpret_cast<const char *>(ptr->text.data), static_cast<int>(ptr->text.length));
            return QVariant(resStr);
        }
        else if (out.type == &UA_TYPES[UA_TYPES_UINT32])
        {
            UA_UInt32 * ptr = (UA_UInt32 *)out.data;
            // printf("UInt32 Value: %d\n", *ptr);
            quint32 resData = *ptr;
            return QVariant(resData);
        }
        else if (out.type == &UA_TYPES[UA_TYPES_INT32])
        {
            UA_Int32 * ptr = (UA_Int32 *)out.data;
            // printf("Int32 Value: %d\n", *ptr);
            qint32 resData = *ptr;
            return QVariant(resData);
        }
        else if (out.type == &UA_TYPES[UA_TYPES_DOUBLE])
        {
            UA_Double * ptr = (UA_Double *)out.data;
            // printf("UA_Double Value: %.2f\n", *ptr);
            double resData = *ptr;
            return QVariant(resData);
        }
    }

    UA_ReadResponse_clear(&response); // 清除响应，释放内存
    return QVariant();
}

void OpcUaClientSync::writeValue(const QString & fullNodeId, QVariant & value)
{
    UA_WriteValue wValueArray[1]; // 写入数组

    UA_WriteValue_init(&wValueArray[0]);
    wValueArray[0].attributeId = UA_ATTRIBUTEID_VALUE; // 写入“值”属性

    // 设置 NodeId

    QByteArray nodeNameUtf8 = fullNodeId.toUtf8();
    wValueArray[0].nodeId   = UA_NODEID_STRING(1, const_cast<char *>(nodeNameUtf8.constData()));

    UA_Variant infoVar;
    UA_Variant_init(&infoVar);

    if (value.type() == QVariant::Double)
    {
        UA_Double dataValue = value.toDouble();
        UA_Variant_setScalar(&infoVar, &dataValue, &UA_TYPES[UA_TYPES_DOUBLE]);
    }
    else if (value.type() == QVariant::UInt)
    {
        UA_UInt32 dataValue = value.toUInt();
        UA_Variant_setScalar(&infoVar, &dataValue, &UA_TYPES[UA_TYPES_UINT32]);
    }
    else if (value.type() == QVariant::Int)
    {
        UA_Int32 dataValue = value.toInt();
        UA_Variant_setScalar(&infoVar, &dataValue, &UA_TYPES[UA_TYPES_INT32]);
    }
    else
    {
        // todo 有需要再补充
    }

    wValueArray[0].value.value    = infoVar;
    wValueArray[0].value.hasValue = true;

    // 构造写入请求
    UA_WriteRequest wReq;
    UA_WriteRequest_init(&wReq);
    wReq.nodesToWrite     = &wValueArray[0];
    wReq.nodesToWriteSize = 1;

    // 发送写入请求
    UA_WriteResponse wResp = UA_Client_Service_write(m_client, wReq);

    // 检查服务返回状态
    if (wResp.responseHeader.serviceResult == UA_STATUSCODE_GOOD && wResp.resultsSize > 0 && wResp.results[0] == UA_STATUSCODE_GOOD)
    {
        // do nothing
    }
    else
    {
        qDebug() << "修改节点数据失败(网络错误/读写权限/数据类型可能不匹配): " << fullNodeId;
    }

    UA_WriteResponse_clear(&wResp); // 清除响应
}

bool OpcUaClientSync::canWriteValue(const QString & parentNode, const QString & subNode, QVariant & value)
{
    for (DeviceConfig device : m_config.devices)
    {
        if (device.device_node_id == parentNode)
        {
            for (VariableConfig variable : device.variables)
            {
                if (variable.browse_name == subNode)
                {
                    QString fullNodeId = parentNode + "_" + subNode;
                    writeValue(fullNodeId, value);
                    return true;
                }
            }
            break;
        }
    }
    return false;
}

bool OpcUaClientSync::hasNode(const QString & parentNodeId, const QString & subNodeId)
{
    for (DeviceConfig device : m_config.devices)
    {
        if (device.device_node_id == parentNodeId)
        {
            for (VariableConfig variable : device.variables)
            {
                if (variable.browse_name == subNodeId)
                {
                    return true;
                }
            }
            break;
        }
    }
    return false;
}
