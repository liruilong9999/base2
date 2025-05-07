#include "config.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

QVector<OpcUaConfig> ConfigParser::parse(const QString & filePath)
{
    QVector<OpcUaConfig> configs;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning("无法打开配置文件");
        return configs;
    }

    QJsonDocument doc        = QJsonDocument::fromJson(file.readAll());
    QJsonObject   root       = doc.object();
    QJsonArray    opcuaArray = root["opcua"].toArray();

    for (const QJsonValue & configVal : opcuaArray)
    {
        QJsonObject configObj = configVal.toObject();
        OpcUaConfig config;
        config.url    = configObj["url"].toString();
        config.device = configObj["device"].toString();
        config.period = configObj["period"].toInt();

        QJsonArray dataArray = configObj["data"].toArray();
        for (const QJsonValue & dataVal : dataArray)
        {
            QJsonObject dataObj = dataVal.toObject();
            NodeConfig  node;
            node.nodeId          = dataObj["node_id"].toString();
            node.nodeClass       = dataObj["node_class"].toString();
            node.browseName      = dataObj["browse_name"].toString();
            node.displayName     = dataObj["display_name"].toString();
            node.dataType        = dataObj["data_type"].toString();
            node.arrayDimensions = dataObj["array_dimensions"].toString().toInt();
            node.defaultValue    = dataObj["default_value"].toVariant();
            node.accessLevel     = dataObj["access_level"].toString();
            node.minValue        = dataObj["min_value"].toDouble();
            node.maxValue        = dataObj["max_value"].toDouble();
            node.description     = dataObj["description"].toString();
            config.data.append(node);
        }
        configs.append(config);
    }

    return configs;
}