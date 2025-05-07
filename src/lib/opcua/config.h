#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QJsonObject>
#include <QVector>
#include <QVariant>

// ���嵥�����ݽڵ��������Ϣ
struct NodeConfig
{
    QString  nodeId;          // �ڵ�ID���� "4100101"��
    QString  nodeClass;       // �ڵ����ͣ�"variable" �� "object"��
    QString  browseName;      // ������ƣ��� "status"��
    QString  displayName;     // ��ʾ���ƣ��� "ϵ�y״̬"��
    QString  dataType;        // �������ͣ��� "int32"��
    int      arrayDimensions; // ����ά�ȣ�0��ʾ������
    QVariant defaultValue;    // Ĭ��ֵ
    QString  accessLevel;     // ����Ȩ�ޣ�"rw" �� "ro"��
    double   minValue;        // ��Сֵ��������ֵ���ͣ�
    double   maxValue;        // ���ֵ��������ֵ���ͣ�
    QString  description;     // ������Ϣ
};

// ����OPC UA��������������Ϣ
struct OpcUaConfig
{
    QString             url;    // ��������ַ���� "opc.tcp://127.0.0.1:4840"��
    QString             device; // �豸���ƣ��� "navigation"��
    int                 period; // ���ݸ������ڣ����룩
    QVector<NodeConfig> data;   // ���ݽڵ��б�
};

class ConfigParser
{
public:
    // ��JSON�ļ���������
    static QVector<OpcUaConfig> parse(const QString & filePath);
};

#endif // CONFIG_H