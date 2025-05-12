

#include <iostream>
#include <QApplication>
#include "opcua/server.h"
#include "opcua/servermanager.h"

#include "opcua/client.h"

/**客户端测试代码*/

int main(int argc, char * argv[])
{
    QApplication a(argc, argv);

    ServerManager m;
    m.loadConfig("./config/opcua.json");
    m.startThread();
    int         ret = 0;
    OpcUaClient client;
    if (client.connectToServer("opc.tcp://127.0.0.1:4840"))
    {
        QVariant val = client.readValue("navigation_SystemStatus");
        qDebug() << "Current:" << val.toDouble();
        client.subscribeNodeValue("navigation_SystemStatus");
        client.subscribeNodeValue("navigation_Speed");
        /*     val = client.readValue("navigation", "SystemStatus", "int32");
             qDebug() << "SystemStatus:" << val.toInt();*/
        ret = a.exec();
        client.disconnectFromServer();
    }
    else
    {
        ret = a.exec();
    }
    return ret;
}