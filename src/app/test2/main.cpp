

#include <iostream>
#include <QApplication>
#include "opcua/server.h"
#include "opcua/servermanager.h"

#include "opcua/client.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ServerManager m;
    m.loadConfig("./config/opcua.json");
    m.startThread();

    OpcUaClient client;
    if (client.connectToServer("opc.tcp://127.0.0.1:4840"))
    {
        QVariant val = client.readValue( "elecSenser/Current");
        qDebug() << "Current:" << val.toDouble();

   /*     val = client.readValue("navigation", "SystemStatus", "int32");
        qDebug() << "SystemStatus:" << val.toInt();*/

        client.disconnectFromServer();
    }

    return a.exec();
}