#include <QDebug>

#include "clientwidget.h"
#include "ui_clientwidget.h"

#include "opcua/opcuaclientmanager.h"

ClientWidget::ClientWidget(QWidget * parent)
    : QWidget(parent)
    , ui(new Ui::ClientWidget)
{
    ui->setupUi(this);

    setFixedSize(700, 800);
    // ui->treeWidget->
    connect(ui->treeWidget, &QTreeWidget::itemChanged, this, &ClientWidget::itemChanged);

    m_pCMa = new OpcUaClientManager(this);

    QString path = "./config/opcua.json";
    m_pCMa->loadConfig(path);
    m_pCMa->startThread();
    bool isSuccess = ConfigParser::parseConfig(path, m_config);
    if (!isSuccess)
        return;

    ui->treeWidget->clear();
    for (auto conf : m_config)
    {
        populateTreeWidget(ui->treeWidget, conf.devices);
    }

    eventSubscribe(CLIENT_PUB_SUB_TOPIC, SLOT(updateSubClientData), this);

    // 注册所有回调
    subAllNode(); // 这里需要注意下，保障客户端线程run函数先执行后再执行该函数，因为run中有信号槽
}

void ClientWidget::updateTreeWidgetValue(QTreeWidget * treeWidget, const QString & parentNode, const QString & subNode, const QVariant & value)
{
    int nameColumn  = 0; // 节点名所在列
    int valueColumn = 3; // Value 列所在索引（根据实际 UI 定）

    QTreeWidgetItem * targetParent = nullptr;

    // 1. 找 parentNode
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem * item = treeWidget->topLevelItem(i);
        if (item->data(nameColumn, Qt::UserRole + 1) == parentNode)
        {
            targetParent = item;
            break;
        }
    }

    if (!targetParent)
    {
        qWarning() << "未找到父节点:" << parentNode;
        return;
    }

    // 2. 找 subNode 子节点
    for (int j = 0; j < targetParent->childCount(); ++j)
    {
        QTreeWidgetItem * child = targetParent->child(j);
        if (child->data(nameColumn, Qt::UserRole + 2) == subNode)
        {
            child->setText(valueColumn, value.toString());
            return;
        }
    }

    qWarning() << "未找到子节点:" << subNode << "父节点为" << parentNode;
}

void ClientWidget::subAllNode()
{
    for (OpcUaConfig clientConf : m_config)
    {
        for (DeviceConfig device : clientConf.devices)
        {
            QString parentNode = device.device_node_id;
            for (VariableConfig variable : device.variables)
            {
                QString subNode = variable.browse_name;
                if (m_pCMa)
                {
                    m_pCMa->subscribeNodeValue(parentNode, subNode);
                }
            }
        }
    }
}

void ClientWidget::updateSubClientData(const QVariant & var)
{
    if (var.canConvert<OpcuaPubParam>())
    {
        OpcuaPubParam params   = var.value<OpcuaPubParam>();
        QStringList   nodeList = params.subNode.split("_");
        if (nodeList.size() >= 2)
        {
            QString parentNode = nodeList[0];
            QString subNode    = nodeList[1];
            // 更新一个item
            updateTreeWidgetValue(ui->treeWidget, parentNode, subNode, params.value);
        }
    }
}

void ClientWidget::populateTreeWidget(QTreeWidget * treeWidget, const QVector<DeviceConfig> & devices)
{
    for (const auto & device : devices)
    {
        QTreeWidgetItem * deviceItem = new QTreeWidgetItem(treeWidget);
        deviceItem->setText(0, device.device_node_id);
        QVariant deviceVar = QVariant::fromValue<QString>(device.device_node_id);
        deviceItem->setData(0, Qt::UserRole + 1, deviceVar);

        for (const auto & variable : device.variables)
        {
            QTreeWidgetItem * varItem = new QTreeWidgetItem(deviceItem);
            varItem->setText(0, variable.browse_name);
            varItem->setText(1, variable.display_name);
            varItem->setText(2, variable.data_type);
            varItem->setText(3, variable.default_value.toString());

            QVariant browseNameVar = QVariant::fromValue<QString>(variable.browse_name);
            varItem->setData(0, Qt::UserRole + 2, browseNameVar);
            varItem->setData(0, Qt::UserRole + 3, QVariant::fromValue<QString>(QString(device.device_node_id + "." + variable.browse_name)));
            varItem->setData(2, Qt::UserRole + 4, QVariant::fromValue<QString>(variable.data_type));

            // 设置为可编辑项
            varItem->setFlags(varItem->flags() | Qt::ItemIsEditable);

            // 存储完整 NodeId 信息作为 data
            varItem->setData(0, Qt::UserRole, device.device_node_id + "_" + variable.browse_name);
        }
    }

    treeWidget->setHeaderLabels(QStringList{"名称", "BrowseName", "类型", "值"});
    treeWidget->expandAll();
}

ClientWidget::~ClientWidget()
{
    delete ui;
}

void ClientWidget::itemChanged(QTreeWidgetItem * item, int column)
{
    if (column != 3 && !m_pCMa)
        return; // 只处理值列

    QString     fullNodeIdStr = item->data(0, Qt::UserRole + 3).toString();
    QStringList parts         = fullNodeIdStr.split(".");
    if (parts.size() != 2)
        return;

    QString deviceNodeId = parts[0];
    QString browseName   = parts[1];
    QString newValueStr  = item->text(3);

    // 构造完整 nodeId（示例）
    QString  unit = item->data(2, Qt::UserRole + 4).toString(); // 数据类型
    QVariant newValue;
    if (unit == "uint32")
    {
        newValue = QVariant::fromValue<uint32_t>(newValueStr.toInt());
    }
    else if (unit == "int32")
    {
        newValue = QVariant::fromValue<int32_t>(newValueStr.toInt());
    }
    else if (unit == "double")
    {
        newValue = QVariant::fromValue<double>(newValueStr.toDouble());
    }
    else
    {
        return;
    }

    m_pCMa->writeValue(deviceNodeId, browseName, newValue);
}