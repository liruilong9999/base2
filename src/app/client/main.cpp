#include <iostream>
#include <QApplication>

#include "clientwidget.h"

int main(int argc, char * argv[])
{
    QApplication a(argc, argv);

	ClientWidget cw;
	cw.show();

    return a.exec();
}

//
//void updateData()
//{
//    ClientManager m;
//    m.loadConfig("./config/opcua.json");
//    m.startThread();
//
//    std::pair<bool, QVariant> res1 = m.getNodeData("Navigation", "SystemStatus");
//    if (res1.first == true)
//    {
//        qDebug() << "__________1" << res1.second;
//    }
//
//    m.writeValue("Navigation", "SystemStatus", QVariant::fromValue<uint32_t>(25));
//
//    res1 = m.getNodeData("Navigation", "SystemStatus");
//    if (res1.first == true)
//    {
//        qDebug() << "__________2" << res1.second;
//    }
//
//    m.subscribeNodeValue("Navigation", "SystemStatus");
//    m.subscribeNodeValue("Navigation", "Speed");
//}
