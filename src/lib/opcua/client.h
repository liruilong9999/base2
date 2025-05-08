#ifndef OPCUACLIENT_H
#define OPCUACLIENT_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>
#include "config.h" // 你提供的配置结构
#include <open62541/client.h>
#include <open62541/client_config_default.h>

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

private:
    UA_Client * m_client = nullptr;
};

#endif // OPCUACLIENT_H
