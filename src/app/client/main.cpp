#include <stdlib.h>
#include <stdio.h>
#include "open62541.h"

static UA_StatusCode multiRead(UA_Client * client)
{
    const int      arraySize = 1;
    UA_ReadValueId itemArray[arraySize];
    for (int i = 0; i < arraySize; ++i)
    {
        UA_ReadValueId_init(&itemArray[i]);
        itemArray[i].attributeId = UA_ATTRIBUTEID_VALUE;
    }

    itemArray[0].nodeId = UA_NODEID_STRING(1, "navigation_SystemStatus");

    UA_ReadRequest request;
    UA_ReadRequest_init(&request);
    request.nodesToRead     = &itemArray[0];
    request.nodesToReadSize = arraySize;

    UA_ReadResponse response = UA_Client_Service_read(client, request);

    UA_StatusCode retStatusArray[arraySize];
    UA_StatusCode retval = response.responseHeader.serviceResult;
    if (retval == UA_STATUSCODE_GOOD)
    {
        if (response.resultsSize == arraySize)
        {
            for (int i = 0; i < arraySize; ++i)
            {
                retStatusArray[i] = response.results[i].status;
            }
        }
        else
        {
            UA_ReadResponse_clear(&response);
            return UA_STATUSCODE_BADUNEXPECTEDERROR;
        }
    }

    for (int i = 0; i < arraySize; ++i)
    {
        if (retStatusArray[i] == UA_STATUSCODE_GOOD)
        {
            UA_DataValue res = response.results[i];
            if (!res.hasValue) // no value
            {
                UA_ReadResponse_clear(&response);
                return UA_STATUSCODE_BADUNEXPECTEDERROR;
            }

            UA_Variant out;
            memcpy(&out, &res.value, sizeof(UA_Variant));
            UA_Variant_init(&res.value);

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
                printf("UInt32 Value: %d\n", *ptr);
            }
        }
    }

    UA_ReadResponse_clear(&response);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode multiWrite(UA_Client * client, UA_UInt32 Index)
{
    const int     arraySize = 1;
    UA_WriteValue wValueArray[arraySize];
    for (int i = 0; i < arraySize; ++i)
    {
        UA_WriteValue_init(&wValueArray[i]);
        wValueArray[i].attributeId = UA_ATTRIBUTEID_VALUE;
    }

    wValueArray[0].nodeId = UA_NODEID_STRING(1, "navigation_SystemStatus");
    UA_Variant       infoVar;
    
    UA_Int32 uint1Value = 101 + Index;
    UA_Variant_init(&infoVar);
    UA_Variant_setScalar(&infoVar, &uint1Value, &UA_TYPES[UA_TYPES_INT32]);
    wValueArray[0].value.value    = infoVar;
    wValueArray[0].value.hasValue = true;

    UA_WriteRequest wReq;
    UA_WriteRequest_init(&wReq);
    wReq.nodesToWrite     = &wValueArray[0];
    wReq.nodesToWriteSize = arraySize;

    UA_WriteResponse wResp = UA_Client_Service_write(client, wReq);

    UA_StatusCode retval = wResp.responseHeader.serviceResult;
    if (retval == UA_STATUSCODE_GOOD)
    {
        if (wResp.resultsSize == 1)
            retval = wResp.results[0];
        else
            retval = UA_STATUSCODE_BADUNEXPECTEDERROR;
    }

    UA_WriteResponse_clear(&wResp);
    return retval;
}

int main(void)
{
    UA_UInt32 mIndex;
    mIndex             = 0;
    UA_Client * client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://localhost:4840");
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_Client_delete(client);
        return (int)retval;
    }
    while (1)
    {
        printf("---- Before write ---- \n");
        multiRead(client);

        printf("\n\n");
        multiWrite(client, mIndex);

        printf("---- After write ---- \n");
        multiRead(client);
        mIndex++;
    }

    UA_Client_delete(client); /* Disconnects the client internally */

    return EXIT_SUCCESS;
}