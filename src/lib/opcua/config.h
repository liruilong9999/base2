#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QJsonObject>
#include <QVector>
#include <QVariant>

// 定义单个数据节点的配置信息
struct NodeConfig
{
    QString  nodeId;          // 节点ID（如 "4100101"）
    QString  nodeClass;       // 节点类型（"variable" 或 "object"）
    QString  browseName;      // 浏览名称（如 "status"）
    QString  displayName;     // 显示名称（如 "系統状态"）
    QString  dataType;        // 数据类型（如 "int32"）
    int      arrayDimensions; // 数组维度（0表示标量）
    QVariant defaultValue;    // 默认值
    QString  accessLevel;     // 访问权限（"rw" 或 "ro"）
    double   minValue;        // 最小值（仅限数值类型）
    double   maxValue;        // 最大值（仅限数值类型）
    QString  description;     // 描述信息
};

// 定义OPC UA服务器的配置信息
struct OpcUaConfig
{
    QString             url;    // 服务器地址（如 "opc.tcp://127.0.0.1:4840"）
    QString             device; // 设备名称（如 "navigation"）
    int                 period; // 数据更新周期（毫秒）
    QVector<NodeConfig> data;   // 数据节点列表
};

class ConfigParser
{
public:
    // 从JSON文件解析配置
    static QVector<OpcUaConfig> parse(const QString & filePath);
};

#endif // CONFIG_H