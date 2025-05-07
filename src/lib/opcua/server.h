#ifndef SERVER_H
#define SERVER_H

#include <open62541/server.h>
#include "config.h"

class OpcUaServer
{
public:
    OpcUaServer();
    ~OpcUaServer();

    // 启动服务器并加载配置
    bool start( QVector<OpcUaConfig> & configs);
    // 停止服务器
    void stop();

private:
    UA_Server * m_server;  // OPC UA 服务器实例
    bool        m_running; // 服务器运行状态

    // 创建节点
    void createNodes( OpcUaConfig & config);
    // 数据类型映射
    const UA_DataType * mapDataType( QString & typeStr);
    // 访问权限映射
    UA_Byte mapAccessLevel( QString & accessLevel);
};

#endif // SERVER_H