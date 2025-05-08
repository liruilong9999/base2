#include <stdlib.h>
#include <stdio.h>
#include "open62541.h"

// 多节点读取函数
static UA_StatusCode multiRead(UA_Client * client)
{
    const int      arraySize = 1;
    UA_ReadValueId itemArray[arraySize]; // 存储需要读取的节点信息

    // 初始化每个要读取的节点
    for (int i = 0; i < arraySize; ++i)
    {
        UA_ReadValueId_init(&itemArray[i]);
        itemArray[i].attributeId = UA_ATTRIBUTEID_VALUE; // 指定读取的是“值”属性
    }

    // 设置各节点的 NodeId
    itemArray[0].nodeId = UA_NODEID_STRING(1, "navigation_Speed");
    // itemArray[1].nodeId = UA_NODEID_STRING(1, "info2");
    // itemArray[2].nodeId = UA_NODEID_STRING(1, "info3");
    // itemArray[3].nodeId = UA_NODEID_STRING(1, "uint1");
    // itemArray[4].nodeId = UA_NODEID_STRING(1, "uint2");
    // itemArray[5].nodeId = UA_NODEID_STRING(1, "uint3");

    // 创建读取请求
    UA_ReadRequest request;
    UA_ReadRequest_init(&request);
    request.nodesToRead     = &itemArray[0];
    request.nodesToReadSize = arraySize;

    // 发送读取请求
    UA_ReadResponse response = UA_Client_Service_read(client, request);

    UA_StatusCode retStatusArray[arraySize];
    UA_StatusCode retval = response.responseHeader.serviceResult;

    // 判断整体请求是否成功
    if (retval == UA_STATUSCODE_GOOD)
    {
        // 检查结果数量是否与请求数量一致
        if (response.resultsSize == arraySize)
        {
            for (int i = 0; i < arraySize; ++i)
            {
                retStatusArray[i] = response.results[i].status; // 记录每个节点的读取状态码
            }
        }
        else
        {
            UA_ReadResponse_clear(&response);
            return UA_STATUSCODE_BADUNEXPECTEDERROR; // 结果数量不对
        }
    }

    // 遍历每个读取结果
    for (int i = 0; i < arraySize; ++i)
    {
        if (retStatusArray[i] == UA_STATUSCODE_GOOD)
        {
            UA_DataValue res = response.results[i];

            if (!res.hasValue) // 没有返回值
            {
                UA_ReadResponse_clear(&response);
                return UA_STATUSCODE_BADUNEXPECTEDERROR;
            }

            UA_Variant out;
            memcpy(&out, &res.value, sizeof(UA_Variant)); // 拷贝 Variant
            UA_Variant_init(&res.value);                  // 清空原值，避免重复释放

            // 判断数据类型
            if (out.type == &UA_TYPES[UA_TYPES_LOCALIZEDTEXT])
            {
                UA_LocalizedText * ptr = (UA_LocalizedText *)out.data;
                printf("Text: %.*s\n", ptr->text.length, ptr->text.data);
            }
            else if (out.type == &UA_TYPES[UA_TYPES_UINT32])
            {
                UA_UInt32 * ptr = (UA_UInt32 *)out.data;
                printf("UInt32 Value: %d\n", *ptr);
            }
            else if (out.type == &UA_TYPES[UA_TYPES_INT32])
            {
                UA_Int32 * ptr = (UA_Int32 *)out.data;
                printf("Int32 Value: %d\n", *ptr);
            }
            else if (out.type == &UA_TYPES[UA_TYPES_DOUBLE])
            {
                UA_Double * ptr = (UA_Double *)out.data;
                printf("UA_Double Value: %.2f\n", *ptr);
            }
        }
    }

    UA_ReadResponse_clear(&response); // 清除响应，释放内存
    return UA_STATUSCODE_GOOD;
}

// 多节点写入函数
UA_StatusCode multiWrite(UA_Client * client, UA_Double Index)
{
    const int     arraySize = 1;
    UA_WriteValue wValueArray[arraySize]; // 写入数组

    // 初始化写入项
    for (int i = 0; i < arraySize; ++i)
    {
        UA_WriteValue_init(&wValueArray[i]);
        wValueArray[i].attributeId = UA_ATTRIBUTEID_VALUE; // 写入“值”属性
    }

    // 设置 NodeId
    wValueArray[0].nodeId = UA_NODEID_STRING(1, "navigation_Speed");
    // wValueArray[1].nodeId = UA_NODEID_STRING(1, "info2");
    // wValueArray[2].nodeId = UA_NODEID_STRING(1, "info3");
    // wValueArray[3].nodeId = UA_NODEID_STRING(1, "uint1");
    // wValueArray[4].nodeId = UA_NODEID_STRING(1, "uint2");
    // wValueArray[5].nodeId = UA_NODEID_STRING(1, "uint3");

    UA_Variant infoVar;

    // 设置 info1 的值为 "world1"
    // 设置 uint1 的值
    UA_Double uint1Value = 11.00 + Index;
    UA_Variant_init(&infoVar);
    UA_Variant_setScalar(&infoVar, &uint1Value, &UA_TYPES[UA_TYPES_DOUBLE]);
    wValueArray[0].value.value    = infoVar;
    wValueArray[0].value.hasValue = true;

    // 构造写入请求
    UA_WriteRequest wReq;
    UA_WriteRequest_init(&wReq);
    wReq.nodesToWrite     = &wValueArray[0];
    wReq.nodesToWriteSize = arraySize;

    // 发送写入请求
    UA_WriteResponse wResp = UA_Client_Service_write(client, wReq);

    UA_StatusCode retval = wResp.responseHeader.serviceResult;

    // 检查服务返回状态
    if (retval == UA_STATUSCODE_GOOD)
    {
        if (wResp.resultsSize == 1)
            retval = wResp.results[0];
        else
            retval = UA_STATUSCODE_BADUNEXPECTEDERROR;
    }

    UA_WriteResponse_clear(&wResp); // 清除响应
    return retval;
}

int main(void)
{
    UA_Double mIndex;
    mIndex = 0;

    UA_Client * client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));

    // 连接到本地 OPC UA 服务器
    UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://localhost:4840");
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_Client_delete(client);
        return (int)retval;
    }

    // 进入循环：读取、写入、再次读取
    while (1)
    {
        printf("---- Before write ---- \n");
        multiRead(client);

        printf("\n\n");
        multiWrite(client, mIndex);

        printf("---- After write ---- \n");
        multiRead(client);
        mIndex += 0.1;
    }

    // 删除客户端，断开连接
    UA_Client_delete(client);

    return EXIT_SUCCESS;
}
