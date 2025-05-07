#ifndef SERVER_H
#define SERVER_H

#include <open62541/server.h>
#include "config.h"

class OpcUaServer
{
public:
    OpcUaServer();
    ~OpcUaServer();

    // ��������������������
    bool start( QVector<OpcUaConfig> & configs);
    // ֹͣ������
    void stop();

private:
    UA_Server * m_server;  // OPC UA ������ʵ��
    bool        m_running; // ����������״̬

    // �����ڵ�
    void createNodes( OpcUaConfig & config);
    // ��������ӳ��
    const UA_DataType * mapDataType( QString & typeStr);
    // ����Ȩ��ӳ��
    UA_Byte mapAccessLevel( QString & accessLevel);
};

#endif // SERVER_H