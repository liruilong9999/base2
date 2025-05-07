#ifndef CLIENT_H
#define CLIENT_H

#include <open62541/client.h>
#include <QObject>
#include "config.h"

class OpcUaClient : public QObject
{
    Q_OBJECT
public:
    explicit OpcUaClient(QObject * parent = nullptr);
    ~OpcUaClient();

    // ���ӷ�����
    bool connect(const QString & url);
    // �Ͽ�����
    void disconnect();
    // д������
    bool writeValue(const QString & nodeId, const QVariant & value);
    // �������ݱ��
    void subscribe(const QString & nodeId, int intervalMs);

signals:
    void valueChanged(const QString & nodeId, const QVariant & value);

private:
    UA_Client * m_client; // OPC UA �ͻ���ʵ��
};

#endif // CLIENT_H