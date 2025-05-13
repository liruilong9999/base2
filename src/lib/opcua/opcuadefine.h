#ifndef define_h
#define define_h

#include <QString>
#include <QVariant>
#include <QMetaType>

struct OpcuaPubParam
{
    QString  subNode;
    QVariant value;
};
Q_DECLARE_METATYPE(OpcuaPubParam)

// 接收订阅数据使用该主题
#define CLIENT_PUB_SUB_TOPIC QString("ClientPubSub_Topic")  

#endif