#ifndef CLIENT_H
#define CLIENT_H

#include <open62541/client.h>
#include <QObject>
#include "config.h"
//
//class OpcUaClient : public QObject
//{
//    Q_OBJECT
//public:
//    explicit OpcUaClient(QObject * parent = nullptr);
//    ~OpcUaClient();
//
//    // 连接服务器
//    bool connect(const QString & url);
//    // 断开连接
//    void disconnect();
//    // 写入数据
//    bool writeValue(const QString & nodeId, const QVariant & value);
//    // 订阅数据变更
//    void subscribe(const QString & nodeId, int intervalMs);
//
//signals:
//    void valueChanged(const QString & nodeId, const QVariant & value);
//
//private:
//    UA_Client * m_client; // OPC UA 客户端实例
//};

#endif // CLIENT_H