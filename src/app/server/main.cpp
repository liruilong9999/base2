#include <signal.h>
#include <stdlib.h>

#include "open62541.h"

UA_Boolean running = true;

static void stopHandler(int sign)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

static void addLocalizedTextVariable(UA_Server * server, char * name)
{
    /* Define the attribute of the myInteger variable node */
    UA_VariableAttributes attr = UA_VariableAttributes_default;

    UA_LocalizedText orig = UA_LOCALIZEDTEXT((char *)"en-US", "hello");

    UA_Variant_setScalar(&attr.value, &orig, &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
    attr.description = UA_LOCALIZEDTEXT("en-US", name);
    attr.displayName = UA_LOCALIZEDTEXT("en-US", name);
    attr.dataType    = UA_TYPES[UA_TYPES_LOCALIZEDTEXT].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    /* Add the variable node to the information model */
    UA_NodeId        myNodeId              = UA_NODEID_STRING(1, name);
    UA_QualifiedName myName                = UA_QUALIFIEDNAME(1, name);
    UA_NodeId        parentNodeId          = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId        parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, myNodeId, parentNodeId, parentReferenceNodeId, myName, UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
}

static void addUInt32Variable(UA_Server * server, char * name)
{
    /* Define the attribute of the myInteger variable node */
    UA_VariableAttributes attr = UA_VariableAttributes_default;

    UA_UInt32 orig = 100;

    UA_Variant_setScalar(&attr.value, &orig, &UA_TYPES[UA_TYPES_UINT32]);
    attr.description = UA_LOCALIZEDTEXT("en-US", name);
    attr.displayName = UA_LOCALIZEDTEXT("en-US", name);
    attr.dataType    = UA_TYPES[UA_TYPES_UINT32].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    /* Add the variable node to the information model */
    UA_NodeId        myNodeId              = UA_NODEID_STRING(1, name);
    UA_QualifiedName myName                = UA_QUALIFIEDNAME(1, name);
    UA_NodeId        parentNodeId          = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId        parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, myNodeId, parentNodeId, parentReferenceNodeId, myName, UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
}

int main(void)
{
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    UA_Server * server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));

    addLocalizedTextVariable(server, "info1");
    addLocalizedTextVariable(server, "info2");
    addLocalizedTextVariable(server, "info3");

    addUInt32Variable(server, "uint1");
    addUInt32Variable(server, "uint2");
    addUInt32Variable(server, "uint3");

    UA_StatusCode retval = UA_Server_run(server, &running);

    UA_Server_delete(server);

    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}