#ifndef OpcUaClientAsyn_H
#define OpcUaClientAsyn_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>
#include <open62541/client.h>
#include <open62541/client_config_default.h>

#include "opcuaconfig.h"
#include "opcua_global.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	 opcua异步客户端. </summary>
///
/// <remarks>	lrl, 2025/5/13. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class OPCUA_EXPORT OpcUaClientAsyn : public QObject
{
    Q_OBJECT

public:
    explicit OpcUaClientAsyn(OpcUaConfig config, QString url, QObject * parent = nullptr);
    ~OpcUaClientAsyn();

    bool connectToServer();

    void subscribeNodeValue(const QString & parentNodeId, const QString & subNodeId);

    bool hasNode(const QString & parentNodeId, const QString & subNodeId);

private:
    UA_Client * m_client = nullptr;
    QString     m_url;    // 每一个客户端，对应一个服务端ip，多个服务端时需要创建多个客户端类或者线程
    OpcUaConfig m_config; // 配置文件参数
};

#endif // OpcUaClientAsyn_H
