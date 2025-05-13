#ifndef OpcUaClientManager_H__20250508
#define OpcUaClientManager_H__20250508

#include <QObject>
#include <QMap>
#include <QVector>
#include <QVariant>
#include <QThread>
#include <QMutex>

#include "opcuaconfig.h"
#include "opcuaclientasyn.h"
#include "opcuaclientsync.h"
#include "opcua_global.h"

class OPCUA_EXPORT OpcUaClientManager : public QThread
{
    Q_OBJECT
public:
    explicit OpcUaClientManager(QObject * parent = nullptr);
    ~OpcUaClientManager() override;

    void loadConfig(QString path);

    void startThread();

    std::pair<bool, QVariant> getNodeData(const QString & parentNode, const QString & subNode);

    bool writeValue(const QString & parentNode, const QString & subNode, QVariant & value);

signals:
    void subscribeNodeValue(const QString & parentNodeId, const QString & subNodeId);

public slots:
    bool subscribeNodeValueSlot(const QString & parentNodeId, const QString & subNodeId);

protected:
    void run() override;

private:
    QMap<QString, OpcUaClientSync *> m_syncClients; // 同步客户端列表
    QMap<QString, OpcUaClientAsyn *> m_asynClients; // 异步客户端列表
    QVector<OpcUaConfig>             m_config;      // 配置文件参数
    QMutex                           m_mutex;       // 锁住 m_config
};

#endif
