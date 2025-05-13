#include <iostream>
#include <QApplication>
#include "opcua/opcuaserver.h"
#include "opcua/opcuaservermanager.h"

int main(int argc, char * argv[])
{
    QApplication a(argc, argv);

    OpcUaServerManager m;
    m.loadConfig("./config/opcua.json");
    m.startThread();
    std::vector<UA_NodeId> nodes = {
        UA_NODEID_STRING_ALLOC(1, "Navigation_SystemStatus"),
        UA_NODEID_STRING_ALLOC(1, "Navigation_Speed")};

    m.setupPeriodicNodePublishing(nodes, 2000, "WG_TempPres", "DSW_TempPres");

    return a.exec();
}