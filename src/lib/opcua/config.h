#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QVector>
#include <QVariant>

#include "opcua_global.h"

// 变量节点配置
struct VariableConfig
{
    QString  browse_name;
    QString  display_name;
    QString  data_type;
    QVariant default_value;
    QString  access_level;
    double   min_value{0.0};
    double   max_value{1.0};
    QString  description;
};

// 设备配置
struct DeviceConfig
{
    QString                 device_name;
    QString                 device_node_id;
    int                     period{100};
    QVector<VariableConfig> variables;
};

// OPC UA服务器配置
struct OpcUaConfig
{
    QString               url;
    quint16               port;
    QVector<DeviceConfig> devices;
};

class OPCUA_EXPORT ConfigParser
{
public:
    static bool parseConfig(const QString & filePath, QVector<OpcUaConfig> & outConfigs);
};

#endif // CONFIG_H