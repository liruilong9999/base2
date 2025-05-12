

#include <iostream>
#include <QApplication>
#include "opcua/server.h"
#include "opcua/servermanager.h"

/*服务端测试代码*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ServerManager m;
    m.loadConfig("./config/opcua.json");
    m.startThread();
    std::vector<UA_NodeId> nodes = {
        UA_NODEID_STRING_ALLOC(1, "navigation_SystemStatus"),
        UA_NODEID_STRING_ALLOC(1, "navigation_Speed")};

    m.setupPeriodicNodePublishing(nodes, 2000, "WG_TempPres", "DSW_TempPres");


    return a.exec();
}