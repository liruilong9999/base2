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
static void addUInt32VariableWithTimestamp(UA_Server * server,  char * name, UA_UInt32 nodeIdNum, UA_UInt32 value)
{
    // ���ñ�����Ĭ������
    UA_VariableAttributes attr = UA_VariableAttributes_default;

    // ������ʾ���ƺ�����
    attr.displayName = UA_LOCALIZEDTEXT("zh-CN", name);
    attr.description = UA_LOCALIZEDTEXT("zh-CN", name);
    attr.dataType    = UA_TYPES[UA_TYPES_UINT32].typeId;                   // ָ����������
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE; // �ɶ�д

    // ��ʼ��ֵ�ṹ�� UA_DataValue
    UA_DataValue dataValue;
    UA_DataValue_init(&dataValue);

    // ���ñ�����ֵ
    UA_Variant_setScalar(&dataValue.value, &value, &UA_TYPES[UA_TYPES_UINT32]);
    dataValue.hasValue = true;

    // ����Դʱ���Ϊ��ǰʱ��
    dataValue.sourceTimestamp    = UA_DateTime_now();
    dataValue.hasSourceTimestamp = true;

    // ��������״̬��Ϊ GOOD
    dataValue.status    = UA_STATUSCODE_GOOD;
    dataValue.hasStatus = true;

    // �� UA_DataValue ��ֵ��������
    attr.value = dataValue;

    // ���ýڵ� ID ������
    UA_NodeId        nodeId          = UA_NODEID_NUMERIC(1, nodeIdNum);              // �ڵ� ID������ ns=1;i=1001
    UA_QualifiedName qualifiedName   = UA_QUALIFIEDNAME(1, name);                    // �ڵ��޶�����
    UA_NodeId        parentNodeId    = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER); // ���ڵ�Ϊ Objects �ļ���
    UA_NodeId        referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);     // ��֯��ϵ

    // ��ӱ����ڵ㵽��ַ�ռ�
    UA_Server_addVariableNode(
        server,
        nodeId,
        parentNodeId,
        referenceTypeId,
        qualifiedName,
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        attr,
        NULL,
        NULL);
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

    // ��ӱ����ڵ㣨ʾ����
    addUInt32VariableWithTimestamp(server, "�ٶ�kmh", 1001, 88); // ��� UInt32 �ڵ㣬ֵΪ88
    addUInt32VariableWithTimestamp(server, "��ѹVolt", 1002, 220);
    addUInt32VariableWithTimestamp(server, "����Amp", 1003, 15);

    // ������������ѭ����ֱ�� running Ϊ false
    UA_StatusCode retval = UA_Server_run(server, &running);

    // ���ٷ�����ʵ�����ͷ���Դ
    UA_Server_delete(server);

    // �������н�������˳�״̬��
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
