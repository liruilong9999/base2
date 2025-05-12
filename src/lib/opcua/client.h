#ifndef OPCUACLIENT_H
#define OPCUACLIENT_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>

#include <vector>

#include <open62541/client.h>
#include <open62541/client_config_default.h>

#include "config.h" // 配置结构
#include "opcua_global.h"

class OPCUA_EXPORT OpcUaClient : public QObject
{
    Q_OBJECT

public:
    explicit OpcUaClient(QObject * parent = nullptr);
    ~OpcUaClient();

    bool connectToServer(const QString & url);
    void disconnectFromServer();

    QVariant readValue(const QString & fullNodeId);

    void subscribeNodeValue(const QString & nodeIdStr);

private:
    UA_Client * m_client = nullptr;
};

#endif // OPCUACLIENT_H
