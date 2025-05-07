#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QObject>
#include <QMap>
#include <QVector>

#include "config.h"
#include "server.h"
#include "opcua_global.h"

class OPCUA_EXPORT ServerManager : public QObject
{
    Q_OBJECT
public:
    explicit ServerManager(QObject * parent = nullptr);
    ~ServerManager() override;

    void loadConfig(QString path);

    void startThread();

private:
    QMap<QString, OpcUaServer *> m_servers;
    QVector<OpcUaConfig>         m_config;
};

#endif // !1
