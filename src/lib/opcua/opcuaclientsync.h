#ifndef OpcUaClientSync_H
#define OpcUaClientSync_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>
#include <open62541/client.h>
#include <open62541/client_config_default.h>

#include "opcuaconfig.h"
#include "opcua_global.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	同步opcua客户端. </summary>
///
/// <remarks>	lrl, 2025/5/13. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class OPCUA_EXPORT OpcUaClientSync : public QObject
{
    Q_OBJECT

public:
    explicit OpcUaClientSync(OpcUaConfig config, QString url, QObject * parent = nullptr);
    ~OpcUaClientSync();

    bool connectToServer();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	获取节点数据. </summary>
    ///
    /// <remarks>	lrl, 2025/5/13. </remarks>
    ///
    /// <param name="parentNodeId">	父节点字符串. </param>
    /// <param name="subNodeId">   	子节点字符串. </param>
    ///
    /// <returns>	返回数据. </returns>
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    QVariant readValue(const QString & parentNodeId, const QString & subNodeId);

    void writeValue(const QString & fullNodeId, QVariant & value);

    bool canWriteValue(const QString & parentNode, const QString & subNode, QVariant & value);

    bool hasNode(const QString & parentNodeId, const QString & subNodeId);

private:
    UA_Client * m_client = nullptr;
    QString     m_url;    // 每一个客户端，对应一个服务端ip，多个服务端时需要创建多个客户端类或者线程
    OpcUaConfig m_config; // 配置文件参数
};

#endif // OpcUaClientSync_H
