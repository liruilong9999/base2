
#include <QDebug>
#include <open62541/server_config_default.h>
#include <signal.h>
#include <stdlib.h>

#include "translate.h"
#include "server.h"

// ���Ʒ���������״̬�ı�־
UA_Boolean g_running = true;

// Ctrl+C �źŴ�����
static void stopHandler(int sign)
{
    // UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "���յ� Ctrl+C �ж��źţ�׼��ֹͣ������");
    g_running = false;
}

OpcUaServer::OpcUaServer(quint16 port, int period, QObject * parent)
    : QObject(parent)
    , m_period(period)
    , m_port(port)
    , m_pTimer(new QTimer(this))
{
    // ע���źŴ����������� Ctrl+C �ж��ź�
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    // ����������ʵ��
    m_server = UA_Server_new();
    m_config = UA_Server_getConfig(m_server);
}

OpcUaServer::~OpcUaServer()
{
    if (m_server)
    {
        UA_Server_delete(m_server);
    }
}

void OpcUaServer::startServer()
{
    // ���÷����������˿�Ϊ m_port���������� IP��0.0.0.0������ opc.tcp://<����IP>:m_port
    UA_ServerConfig_setMinimal(m_config, m_port, NULL);

    // ��ӱ����ڵ㣨ʾ����

    // ������������ѭ����ֱ�� running Ϊ false
    UA_StatusCode retval = UA_Server_run(m_server, &g_running); // �������������Ҫ�ع�
}

void OpcUaServer::stopServer()
{
    g_running = false;
}

void OpcUaServer::addNode(UA_NodeId node)
{
    m_nodeIds.push_back(node);
}

void OpcUaServer::updateNodeData(const QString & nodeBrowseName, const QVariant & value)
{
}

bool OpcUaServer::createNodes()
{
    return false;
}
