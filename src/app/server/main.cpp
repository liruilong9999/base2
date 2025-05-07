#include <signal.h>
#include <stdlib.h>
#include "open62541.h"

// 控制服务器运行状态的标志
UA_Boolean running = true;

// Ctrl+C 信号处理函数
static void stopHandler(int sign)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "接收到 Ctrl+C 中断信号，准备停止服务器");
    running = false;
}

// 添加一个 UInt32 类型的变量，并设置自定义值、数据类型、时间戳和状态码
// 添加一个带时间戳和状态码的 UInt32 变量节点
static void addUInt32VariableWithTimestamp(UA_Server * server,  char * name, UA_UInt32 nodeIdNum, UA_UInt32 value)
{
    // 1. 创建变量属性
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName           = UA_LOCALIZEDTEXT("zh-CN", name);
    attr.description           = UA_LOCALIZEDTEXT("zh-CN", name);
    attr.dataType              = UA_TYPES[UA_TYPES_UINT32].typeId;
    attr.accessLevel           = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    // 2. 设置初始值（仅值，不含时间戳/状态）
    UA_Variant_setScalar(&attr.value, &value, &UA_TYPES[UA_TYPES_UINT32]);

    // 3. 定义节点信息
    UA_NodeId        nodeId          = UA_NODEID_NUMERIC(1, nodeIdNum); // 自定义节点ID
    UA_QualifiedName qualifiedName   = UA_QUALIFIEDNAME(1, name);
    UA_NodeId        parentNodeId    = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId        referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);

    // 4. 添加变量节点
    UA_Server_addVariableNode(server,
                              nodeId,
                              parentNodeId,
                              referenceTypeId,
                              qualifiedName,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                              attr,
                              NULL,
                              NULL);

    // 5. 设置带时间戳和状态码的数据
    UA_DataValue dataValue;
    UA_DataValue_init(&dataValue);
    UA_Variant_setScalar(&dataValue.value, &value, &UA_TYPES[UA_TYPES_UINT32]);
    dataValue.hasValue = true;

    dataValue.sourceTimestamp    = UA_DateTime_now();
    dataValue.hasSourceTimestamp = true;

    dataValue.status    = UA_STATUSCODE_GOOD;
    dataValue.hasStatus = true;

    // 6. 写入数据（注意：这里是 writeDataValue 而不是 writeValue）
    UA_Server_writeDataValue(server, nodeId, dataValue);
}


int main(void)
{
    // 注册信号处理器，接收 Ctrl+C 中断信号
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    // 创建服务器实例
    UA_Server *       server = UA_Server_new();
    UA_ServerConfig * config = UA_Server_getConfig(server);

    // 设置服务器监听端口为 12345，监听所有 IP（0.0.0.0），即 opc.tcp://<本机IP>:12345
    UA_ServerConfig_setMinimal(config, 12345, NULL);

        // 设置服务器URL
    //UA_String hostname = UA_STRING_ALLOC(m_config.url.toUtf8().constData());
    //UA_ServerConfig_setCustomHostname(UA_Server_getConfig(server), hostname);
    //UA_String_clear(&hostname);


    // 添加变量节点（示例）
    addUInt32VariableWithTimestamp(server, "speed kmh", 1001, 88); // 添加 UInt32 节点，值为88
    addUInt32VariableWithTimestamp(server, "dianya Volt", 1002, 220);
    addUInt32VariableWithTimestamp(server, "dianliu Amp", 1003, 15);

    // 启动服务器主循环，直到 running 为 false
    UA_StatusCode retval = UA_Server_run(server, &running);

    // 销毁服务器实例，释放资源
    UA_Server_delete(server);

    // 根据运行结果返回退出状态码
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
