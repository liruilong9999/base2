#ifndef CLIENTWIDGET_H
#define CLIENTWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <opcua/opcuaconfig.h>
#include <opcua/opcuaclientmanager.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class ClientWidget;
}
QT_END_NAMESPACE

/* 客户端 示例代码 客户端可以启动多个*/
class ClientWidget : public QWidget
{
    Q_OBJECT

public:
    ClientWidget(QWidget * parent = nullptr);
    void populateTreeWidget(QTreeWidget * treeWidget, const QVector<DeviceConfig> & devices);
    ~ClientWidget();

    void updateTreeWidgetValue(QTreeWidget *    treeWidget,
                               const QString &  parentNode,
                               const QString &  subNode,
                               const QVariant & value);
    void subAllNode();

public slots:
    void itemChanged(QTreeWidgetItem * item, int column);
    void updateSubClientData(const QVariant & var);

private:
    Ui::ClientWidget * ui;

    QVector<OpcUaConfig> m_config;

    OpcUaClientManager * m_pCMa{nullptr};
};
#endif // CLIENTWIDGET_H
