

#include <iostream>
#include <QApplication>
#include "opcua/server.h"
#include "opcua/servermanager.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ServerManager m;
    m.loadConfig("./config/opcua.json");
    m.startThread();


    return a.exec();
}