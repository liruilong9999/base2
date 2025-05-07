#include "config.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

bool ConfigParser::parseConfig(const QString & filePath, QVector<OpcUaConfig> & outConfigs)
{
    QFile configFile(filePath);
    if (!configFile.open(QIODevice::ReadOnly))
    {
        qCritical() << "�޷��������ļ�:" << filePath;
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument   doc = QJsonDocument::fromJson(configFile.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        qCritical() << "JSON��������:" << parseError.errorString();
        return false;
    }

    if (!doc.isArray())
    {
        qCritical() << "�����ļ���ʽ���󣺸�Ԫ��ӦΪ����";
        return false;
    }

    QJsonArray rootArray = doc.array();
    for (const QJsonValue & serverVal : rootArray)
    {
        if (!serverVal.isObject())
            continue;

        QJsonObject serverObj = serverVal.toObject();
        OpcUaConfig config;
        config.url = serverObj["url"].toString();

        // ����devices����
        QJsonArray devicesArray = serverObj["devices"].toArray();
        for (const QJsonValue & deviceVal : devicesArray)
        {
            if (!deviceVal.isObject())
                continue;

            QJsonObject  deviceObj = deviceVal.toObject();
            DeviceConfig device;
            device.device_name    = deviceObj["device_name"].toString();
            device.device_node_id = deviceObj["device_node_id"].toString();
            device.period         = deviceObj["period"].toInt();

            // ����variables����
            QJsonArray varsArray = deviceObj["variables"].toArray();
            for (const QJsonValue & varVal : varsArray)
            {
                if (!varVal.isObject())
                    continue;

                QJsonObject    varObj = varVal.toObject();
                VariableConfig variable;
                variable.browse_name   = varObj["browse_name"].toString();
                variable.display_name  = varObj["display_name"].toString();
                variable.data_type     = varObj["data_type"].toString();
                variable.default_value = varObj["default_value"].toVariant();
                variable.access_level  = varObj["access_level"].toString();
                variable.min_value     = varObj["min_value"].toDouble();
                variable.max_value     = varObj["max_value"].toDouble();
                variable.description   = varObj["description"].toString();

                device.variables.append(variable);
            }
            config.devices.append(device);
        }
        outConfigs.append(config);
    }

    return true;
}