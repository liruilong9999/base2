#ifndef OpcUaServerManager_H
#define OpcUaServerManager_H

#include <QObject>
#include <QMap>
#include <QVector>

#include "opcuaconfig.h"
#include "opcuaserver.h"
#include "opcua_global.h"

class OPCUA_EXPORT OpcUaServerManager : public QObject
{
    Q_OBJECT
public:
    explicit OpcUaServerManager(QObject * parent = nullptr);
    ~OpcUaServerManager() override;

    void loadConfig(QString path);

    void startThread();

    void setupPeriodicNodePublishing(const std::vector<UA_NodeId> & nodeList,
                                     double                         intervalMs,
                                     const QString &                writerGroupName,
                                     const QString &                dataSetWriterName);

private:
    QMap<QString, OpcUaServer *> m_servers;
    QVector<OpcUaConfig>         m_config;
};

#endif // !1
