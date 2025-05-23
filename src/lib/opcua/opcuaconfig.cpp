#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QUrl>

#include "opcuaconfig.h"

bool ConfigParser::parseConfig(const QString & filePath, QVector<OpcUaConfig> & outConfigs)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning("Failed to open config file: %s", qPrintable(filePath));
        return false;
    }

    QByteArray      jsonData = file.readAll();
    QJsonParseError parseError;
    QJsonDocument   doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        qWarning("JSON parse error: %s", qPrintable(parseError.errorString()));
        return false;
    }

    if (!doc.isObject())
    {
        qWarning("Root is not a JSON object.");
        return false;
    }

    QJsonObject rootObj    = doc.object();
    QJsonArray  opcuaArray = rootObj.value("opcua").toArray();
    for (const QJsonValue & opcuaVal : opcuaArray)
    {
        QJsonObject opcuaObj = opcuaVal.toObject();
        OpcUaConfig config;
        config.url        = opcuaObj.value("url").toString();
        config.userName   = opcuaObj.value("userName").toString();
        config.password   = opcuaObj.value("password").toString();
        config.maxConnect = opcuaObj.value("password").toInt();
        // 解析 port（从URL中提取）
        QUrl url(config.url);
        config.port = url.port();

        QJsonArray devicesArray = opcuaObj.value("devices").toArray();
        for (const QJsonValue & deviceVal : devicesArray)
        {
            QJsonObject  deviceObj = deviceVal.toObject();
            DeviceConfig device;
            device.device_name    = deviceObj.value("device_name").toString();
            device.device_node_id = deviceObj.value("device_node_id").toString();

            QStringList nodeList = device.device_node_id.split(";");
            if (nodeList.size() < 2)
            {
                qDebug() << "节点ID格式错误：应该为 ns=1;s=ElecSenser 。 ";
                continue;
            }
            QStringList nsList = nodeList[0].split("=");
            if (nsList.size() < 2 || nsList[0] != "ns")
            {
                qDebug() << "节点ID格式错误：应该为 ns=1;s=ElecSenser 。 ";
                continue;
            }
            device.node_ns = nsList[1];

            QStringList ndIdList = nodeList[1].split("=");
            if (ndIdList.size() < 2 || ndIdList[0] != "s")
            {
                qDebug() << "节点ID格式错误：应该为 ns=1;s=ElecSenser 。 ";
                continue;
            }

            device.device_node_id = ndIdList[1];

            device.period = deviceObj.value("period").toInt(100);

            QJsonArray varsArray = deviceObj.value("variables").toArray();
            for (const QJsonValue & varVal : varsArray)
            {
                QJsonObject    varObj = varVal.toObject();
                VariableConfig variable;
                variable.browse_name   = varObj.value("browse_name").toString();
                variable.display_name  = varObj.value("display_name").toString();
                variable.data_type     = varObj.value("data_type").toString();
                variable.default_value = varObj.value("default_value").toVariant();
                variable.access_level  = varObj.value("access_level").toString();
                variable.min_value     = varObj.contains("min_value") ? varObj.value("min_value").toDouble() : 0.0;
                variable.max_value     = varObj.contains("max_value") ? varObj.value("max_value").toDouble() : 1.0;
                variable.description   = varObj.value("description").toString();

                device.variables.append(variable);
            }

            config.devices.append(device);
        }

        outConfigs.append(config);
    }

    return true;
}