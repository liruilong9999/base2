#ifndef SERVER_H
#define SERVER_H

#include <open62541/server.h>
#include "config.h"

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QMap>
#include <QVector>

/// <summary>
/// �����ʾһ���˿ڵķ��񣬸ö˿ڵķ���˿�һ�£����ݷ��ͼ��һ�£������ж���ڵ�
/// </summary>
class OpcUaServer : public QObject
{
    Q_OBJECT
public:
    OpcUaServer(quint16 port, int period, QObject * parent = nullptr);
    ~OpcUaServer() override;

    void startServer();
    void stopServer();

    // ��ӽڵ�,��ǰ����������Щ�ڵ�
    void addNode(UA_NodeId node);

    // ���½ڵ�����
    void updateNodeData(const QString & nodeBrowseName, const QVariant & value);
   
    // �������ô����ڵ�
    bool createNodes();

private:
    double  m_period; // ���ݷ��ͼ������λΪ����
    quint16 m_port;   // �˿�

    QTimer *          m_pTimer; // ��ʱ��
    UA_Server *       m_server; // opcua������
    UA_ServerConfig * m_config; // opcua����

    QMap<UA_NodeId, QVector<UA_Variant>> m_nodeMap; // һ���ڵ��µ����в���(���������ֵ)

    QVector<UA_NodeId> m_nodeIds; // �ڵ�ID�б�
};

#endif // SERVER_H