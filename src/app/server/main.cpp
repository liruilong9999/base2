#include <signal.h>
#include <stdlib.h>
#include "open62541.h"

// ���Ʒ���������״̬�ı�־
UA_Boolean running = true;

// Ctrl+C �źŴ�����
static void stopHandler(int sign)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "���յ� Ctrl+C �ж��źţ�׼��ֹͣ������");
    running = false;
}

// ���һ�� UInt32 ���͵ı������������Զ���ֵ���������͡�ʱ�����״̬��
// ���һ����ʱ�����״̬��� UInt32 �����ڵ�
static void addUInt32VariableWithTimestamp(UA_Server * server,  char * name, UA_UInt32 nodeIdNum, UA_UInt32 value)
{
    // 1. ������������
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName           = UA_LOCALIZEDTEXT("zh-CN", name);
    attr.description           = UA_LOCALIZEDTEXT("zh-CN", name);
    attr.dataType              = UA_TYPES[UA_TYPES_UINT32].typeId;
    attr.accessLevel           = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    // 2. ���ó�ʼֵ����ֵ������ʱ���/״̬��
    UA_Variant_setScalar(&attr.value, &value, &UA_TYPES[UA_TYPES_UINT32]);

    // 3. ����ڵ���Ϣ
    UA_NodeId        nodeId          = UA_NODEID_NUMERIC(1, nodeIdNum); // �Զ���ڵ�ID
    UA_QualifiedName qualifiedName   = UA_QUALIFIEDNAME(1, name);
    UA_NodeId        parentNodeId    = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId        referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);

    // 4. ��ӱ����ڵ�
    UA_Server_addVariableNode(server,
                              nodeId,
                              parentNodeId,
                              referenceTypeId,
                              qualifiedName,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                              attr,
                              NULL,
                              NULL);

    // 5. ���ô�ʱ�����״̬�������
    UA_DataValue dataValue;
    UA_DataValue_init(&dataValue);
    UA_Variant_setScalar(&dataValue.value, &value, &UA_TYPES[UA_TYPES_UINT32]);
    dataValue.hasValue = true;

    dataValue.sourceTimestamp    = UA_DateTime_now();
    dataValue.hasSourceTimestamp = true;

    dataValue.status    = UA_STATUSCODE_GOOD;
    dataValue.hasStatus = true;

    // 6. д�����ݣ�ע�⣺������ writeDataValue ������ writeValue��
    UA_Server_writeDataValue(server, nodeId, dataValue);
}


int main(void)
{
    // ע���źŴ����������� Ctrl+C �ж��ź�
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    // ����������ʵ��
    UA_Server *       server = UA_Server_new();
    UA_ServerConfig * config = UA_Server_getConfig(server);

    // ���÷����������˿�Ϊ 12345���������� IP��0.0.0.0������ opc.tcp://<����IP>:12345
    UA_ServerConfig_setMinimal(config, 12345, NULL);

        // ���÷�����URL
    //UA_String hostname = UA_STRING_ALLOC(m_config.url.toUtf8().constData());
    //UA_ServerConfig_setCustomHostname(UA_Server_getConfig(server), hostname);
    //UA_String_clear(&hostname);


    // ��ӱ����ڵ㣨ʾ����
    addUInt32VariableWithTimestamp(server, "speed kmh", 1001, 88); // ��� UInt32 �ڵ㣬ֵΪ88
    addUInt32VariableWithTimestamp(server, "dianya Volt", 1002, 220);
    addUInt32VariableWithTimestamp(server, "dianliu Amp", 1003, 15);

    // ������������ѭ����ֱ�� running Ϊ false
    UA_StatusCode retval = UA_Server_run(server, &running);

    // ���ٷ�����ʵ�����ͷ���Դ
    UA_Server_delete(server);

    // �������н�������˳�״̬��
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
