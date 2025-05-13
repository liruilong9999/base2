#include <QDebug>
#include <QTimer>
#include <QThread>

extern "C"
{
#include <open62541/client_config_default.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>
}
#include "OpcUaClientAsyn.h"

OpcUaClientAsyn::OpcUaClientAsyn(OpcUaConfig config, QString url, QObject * parent)
    : QObject(parent)
    , m_config(config)
    , m_url(url)
{
    // qDebug() << "__________OpcUaClientAsyn thread______" << QThread::currentThreadId();
    m_client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(m_client));

    // 添加定时器，每 100ms 调用一次 UA_Client_run_iterate() 处理订阅消息
    QTimer * timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (m_client)
        {
            UA_Client_run_iterate(m_client, false);
        }
    });
    timer->start(100); // 调用频率应不低于服务端的发布频率

    // 构造完成，立刻连接服务器，防止异步操作顺序出问题
    // connectToServer();
}

OpcUaClientAsyn::~OpcUaClientAsyn()
{
    if (m_client)
    {
        UA_Client_disconnect(m_client);
        UA_Client_delete(m_client);
    }
}

bool OpcUaClientAsyn::connectToServer()
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

// 回调函数：当订阅节点的值发生变化时会调用此函数
static void handler_DataChanged(UA_Client * client, UA_UInt32 subId, void * subContext, UA_UInt32 monId, void * monContext, UA_DataValue * value)
{
    // qDebug() << "__________handler_DataChanged thread______" << QThread::currentThreadId();
    QString nodeName;
    if (monContext)
    {
        nodeName = *(QString *)monContext;
    }
    else
    {
        nodeName = "未知节点";
    }
    QVariant     dataVar;
    UA_Variant * variant = &value->value;
    if (UA_Variant_hasScalarType(variant, &UA_TYPES[UA_TYPES_DOUBLE]))
    {
        double v = *(UA_Double *)variant->data;
        dataVar  = QVariant::fromValue<double>(v);
        // qDebug() << nodeName << " 值变更：" << v;
    }
    else if (UA_Variant_hasScalarType(variant, &UA_TYPES[UA_TYPES_UINT32]))
    {
        uint32_t v = *(UA_UInt32 *)variant->data;
        dataVar    = QVariant::fromValue<uint32_t>(v);
        // qDebug() << nodeName << " 值变更：" << v;
    }
    else if (UA_Variant_hasScalarType(variant, &UA_TYPES[UA_TYPES_INT32]))
    {
        int32_t v = *(UA_Int32 *)variant->data;
        dataVar   = QVariant::fromValue<int32_t>(v);
        // qDebug() << nodeName << " 值变更：" << v;
    }
    else
    {
        dataVar = QVariant();
        // qDebug() << nodeName << " 接收到其他类型的数据";
    }
    OpcuaPubParam params;
    params.subNode = nodeName;
    params.value   = dataVar;

    QVariant var = QVariant::fromValue<OpcuaPubParam>(params);

    // 订阅的话异步发送数据
    eventPublish(CLIENT_PUB_SUB_TOPIC, var);
}

void OpcUaClientAsyn::subscribeNodeValue(const QString & parentNodeId, const QString & subNodeId)
{
    // qDebug() << "__________OpcUaClientAsyn subscribeNodeValue thread______" << QThread::currentThreadId();
    if (m_config.devices.size() == 0)
        return;

    // 采样间隔
    double period = 100;
    // 获取配置文件中的时间间隔，作为采样间隔和发布间隔，  采样间隔：服务端监测值改变的间隔； 发布间隔:服务端推送值的间隔
    for (DeviceConfig device : m_config.devices)
    {
        // 父节点决定采样间隔
        if (device.device_node_id == parentNodeId)
        {
            period = double(device.period);
            break;
        }
    }

    const QString & nodeIdStr = parentNodeId + "_" + subNodeId;
    // 创建并配置默认客户端
    UA_Client * client = m_client;

    // 解析 nodeId 字符串为 UA_NodeId
    UA_NodeId nodeId = UA_NODEID_STRING_ALLOC(1, nodeIdStr.toUtf8().constData());

    // 创建订阅（发布间隔 100ms）
    UA_CreateSubscriptionRequest request   = UA_CreateSubscriptionRequest_default();
    request.requestedPublishingInterval    = period;
    UA_CreateSubscriptionResponse response = UA_Client_Subscriptions_create(client, request, NULL, NULL, NULL);
    if (response.responseHeader.serviceResult != UA_STATUSCODE_GOOD)
    {
        qWarning("创建订阅失败,请检查网络/ip配置");
        UA_NodeId_clear(&nodeId);
        return;
    }

    UA_UInt32 subId = response.subscriptionId;
    qDebug("成功创建订阅，ID = %u", subId);

    // 设置监控项，订阅该节点的值变化
    UA_MonitoredItemCreateRequest monRequest =
        UA_MonitoredItemCreateRequest_default(nodeId);
    monRequest.requestedParameters.samplingInterval = period; // 采样间隔

    QString * context = new QString(nodeIdStr);

    UA_MonitoredItemCreateResult monResponse =
        UA_Client_MonitoredItems_createDataChange(client,
                                                  subId,
                                                  UA_TIMESTAMPSTORETURN_SOURCE,
                                                  monRequest,
                                                  context, // 增加上下文指针，将节点号放入
                                                  handler_DataChanged,
                                                  NULL);
    if (monResponse.statusCode != UA_STATUSCODE_GOOD)
    {
        qWarning("创建监控项失败");
        UA_NodeId_clear(&nodeId);
        return;
    }

    UA_NodeId_clear(&nodeId);
}

bool OpcUaClientAsyn::hasNode(const QString & parentNodeId, const QString & subNodeId)
{
    // qDebug() << "__________hasNode thread______" << QThread::currentThreadId();
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