#include <QString>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QFileInfoList>

#include <common/IPlugin.h>
// #include <lbase/llog.h>
#include <iostream>
#include <QXmlStreamReader>

#include "pluginmanager.h"

#define PLUGIN_CONF_PATH QString(qApp->applicationDirPath() + "/config/config.xml")

struct PluginCongInfo
{
    QString pluginName{""};
    QString pluginPath{""};
    QString isUsed{"1"};
};

/*!
 * \brief The PluginManagerPrivate class 对已加载的所有插件进行维护
 */
class PluginManagerPrivate
{
public:
    // 插件依赖检测
    bool check(const QString & filepath);

    QHash<QString, QVariant>        m_names;        // 插件路径--插件名称
    QHash<QString, QVariant>        m_versions;     // 插件路径--插件版本
    QHash<QString, QVariantList>    m_dependencies; // 插件路径--插件额外依赖的其他插件
    QHash<QString, QPluginLoader *> m_loaders;      // 插件路径--QPluginLoader实例
    QHash<QString, PluginCongInfo>  pluginCongInfo;
    QStringList                     m_loadOrder; // 插件加载顺序
};

bool PluginManagerPrivate::check(const QString & filepath)
{
    for (QVariant item : m_dependencies.value(filepath))
    {
        QVariantMap map = item.toMap();
        // 依赖的插件名称、版本、路径
        QVariant name    = map.value("name");
        QVariant version = map.value("version");
        QString  path    = m_names.key(name);

        /********** 检测插件是否依赖于其他插件 **********/
        // 先检测插件名称
        if (!m_names.values().contains(name))
        {
            QString strcons = "Missing dependency: " + name.toString() + " for plugin " + path;
            // LOG_INFO(strcons);
            qDebug() << strcons;
            // qDebug() << strcons;
            return false;
        }
        // 再检测插件版本
        if (m_versions.value(path) != version)
        {
            QString strcons = "Version mismatch: " + name.toString() + " version " + m_versions.value(m_names.key(name)).toString() +
                              " but " + version.toString() + " required for plugin " + path;
            // LOG_INFO(strcons);
            qDebug() << strcons;
            // qDebug() << strcons;
            return false;
        }
        // 最后检测被依赖的插件是否还依赖其他的插件
        if (!check(path))
        {
            QString strcons = "Corrupted dependency: " + name.toString() + " for plugin " + path;
            qDebug() << strcons;
            // LOG_INFO(strcons);
            //  qDebug() << strcons;
            return false;
        }
    }

    return true;
}

PluginManager & PluginManager::GetInstance()
{
    static PluginManager instance;
    return instance;
}

bool PluginManager::loadPlugin(QString & filePath)
{
    QString strcons;
    if (!QLibrary::isLibrary(filePath))
    {
        strcons = QString("插件(%1)加载失败，并非加载动态库！").arg(filePath);
        // LOG_WARN(strcons);
        qDebug() << strcons;
        return false;
    }

    // 检测依赖
    if (!m_pluginData && !m_pluginData->check(filePath))
    {
        strcons = QString("插件(%1)加载失败，依赖检查不过关！").arg(filePath);
        // LOG_WARN(strcons);
        qDebug() << strcons;
        return false;
    }

    // 加载插件
    QPluginLoader * loader = new QPluginLoader(filePath);
    QFileInfo       fileInfo(filePath);
    QString         fileName = fileInfo.fileName();
    if (loader && loader->load())
    {
        strcons = QString("加载插件(%1)成功").arg(fileName);
        qDebug() << strcons;
        // LOG_INFO(strcons);
        //  qDebug() << strcons;
        //  std::cout << strcons.toStdString()<<std::endl;
        IPlugin * plugin = qobject_cast<IPlugin *>(loader->instance());
        if (plugin)
        {
            m_pluginData->m_loaders.insert(filePath, loader);
        }
        else
        {
            delete loader;
            loader = nullptr;
        }
        return true;
    }
    strcons = QString("加载插件(%1)失败").arg(fileName);
    qDebug() << strcons;
    // LOG_WARN(strcons);
    //  qDebug() << strcons;
    return false;
}

bool PluginManager::unloadPlugin(QString & filePath)
{
    if (!m_pluginData)
        return false;
    QPluginLoader * loader = m_pluginData->m_loaders.value(filePath);
    QFileInfo       fileInfo(filePath);
    QString         fileName = fileInfo.fileName();

    if (loader && loader->unload())
    {
        m_pluginData->m_loaders.remove(filePath);
        delete loader;
        loader          = nullptr;
        QString strcons = QString("卸载插件(%1)成功").arg(fileName);
        qDebug() << strcons;
        // LOG_INFO(strcons);
        //  qDebug() << strcons;

        return true;
    }
    QString strcons = QString("卸载插件(%1)失败").arg(fileName);
    qDebug() << strcons;
    // LOG_WARN(strcons);
    //  qDebug() << strcons;
    return false;
}

bool PluginManager::loadAllPlugin()
{
    setPluginList();

    // 清空之前加载的插件信息
    m_pluginData->m_loaders.clear();

    QDir pluginsdir = QDir(qApp->applicationDirPath());
    pluginsdir.cd("plugin");
    QFileInfoList pluginsInfo = pluginsdir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    QFileInfoList useableList;

    for (const QFileInfo & fileInfo : pluginsInfo)
    {
        if (QLibrary::isLibrary(fileInfo.absoluteFilePath()))
        {
            QString fileName = fileInfo.baseName(); // 获取插件文件的基本名（不含路径和扩展名）
            if (m_pluginData->pluginCongInfo.contains(fileName))
            {
                PluginCongInfo config = m_pluginData->pluginCongInfo.value(fileName);

                if (config.isUsed == "1")
                {
                    useableList.push_back(fileInfo);
                }
            }
        }
    }

    // 按配置文件中的顺序加载插件
    for (const QString & pluginName : m_pluginData->m_loadOrder)
    {
        bool canLoad = false;
        for (const QFileInfo & fileInfo : useableList)
        {
            if (fileInfo.baseName() == pluginName)
            {
                QString path = fileInfo.absoluteFilePath();
                if (loadPlugin(path))
                {
                    // scanMetaData(path);
                    canLoad = true;
                }
            }
        }
        if (!canLoad)
        {
            QString strcons = QString("未找到插件(%1),动态库为：%2").arg(pluginName).arg(pluginName);
            qDebug() << strcons;
            // LOG_WARN(strcons);
        }
    }

    // 初始化插件
    foreach (const QString & pluginName, m_pluginData->m_loadOrder)
    {
        for (const QFileInfo & fileInfo : useableList)
        {
            if (fileInfo.baseName() == pluginName)
            {
                QString         path   = fileInfo.absoluteFilePath();
                QPluginLoader * loader = m_pluginData->m_loaders.value(path);
                if (loader)
                {
                    IPlugin * plugin = qobject_cast<IPlugin *>(loader->instance());
                    if (plugin)
                    {
                        if (plugin->init())
                        {
                            QString strcons = QString("初始化插件(%1)成功").arg(pluginName);
                            qDebug() << strcons;
                            // LOG_INFO(strcons);
                            //  qDebug() << strcons;
                        }
                        else
                        {
                            QString strcons = QString("初始化插件(%1)失败").arg(pluginName);
                            qDebug() << strcons;
                            // LOG_WARN(strcons);
                            //  qDebug() << strcons;
                        }
                    }
                }
                break;
            }
        }
    }

    return true;
}

bool PluginManager::unloadAllPlugin()
{
    if (!m_pluginData)
        return false;

    // 清理插件
    for (int i = m_pluginData->m_loadOrder.size() - 1; i >= 0; i--)
    {
        for (const QFileInfo & fileInfo : m_pluginData->m_loaders.keys())
        {
            if (fileInfo.baseName() == m_pluginData->m_loadOrder[i])
            {
                QString         path   = fileInfo.absoluteFilePath();
                QPluginLoader * loader = m_pluginData->m_loaders.value(path);
                if (loader)
                {
                    IPlugin * plugin = qobject_cast<IPlugin *>(loader->instance());
                    if (plugin)
                    {
                        if (plugin->clean())
                        {
                            QString strcons = QString("清理插件(%1)成功").arg(fileInfo.baseName());
                            qDebug() << strcons;
                            // LOG_INFO(strcons);
                            //  qDebug() << strcons;
                        }
                        else
                        {
                            QString strcons = QString("初始化插件(%1)失败").arg(fileInfo.baseName());
                            qDebug() << strcons;
                            // LOG_WARN(strcons);
                            //  qDebug() << strcons;
                        }
                    }
                }
                break;
            }
        }
    }
    // 清理插件
    for (int i = m_pluginData->m_loadOrder.size() - 1; i >= 0; i--)
    {
        for (const QFileInfo & fileInfo : m_pluginData->m_loaders.keys())
        {
            if (fileInfo.baseName() == m_pluginData->m_loadOrder[i])
            {
                QString path = fileInfo.absoluteFilePath();
                unloadPlugin(path);
                break;
            }
        }
    }
    return true;
}

QList<QString> PluginManager::getPluginsName()
{
    QList<QString> res;
    return res;
}

void PluginManager::scanMetaData(const QString & filepath)
{
    QString strcons;
    if (!m_pluginData)
    {
        strcons = QString("插件(%1)加载失败，m_pluginData 为空指针！").arg(filepath);
        qDebug() << strcons;
        // LOG_WARN(strcons);
        return;
    }
    if (!QLibrary::isLibrary(filepath))
    {
        strcons = QString("插件(%1)加载失败，并非加载动态库！").arg(filepath);
        qDebug() << strcons;
        // LOG_WARN(strcons);
        return;
    }
    QPluginLoader * loader = new QPluginLoader(filepath);
    if (!loader)
    {
        strcons = QString("插件(%1)加载失败，依赖检查失败！").arg(filepath);
        qDebug() << strcons;
        // LOG_WARN(strcons);
        return;
    }

    QJsonObject json = loader->metaData().value("MetaData").toObject();

    QVariant var = json.value("name").toVariant();
    m_pluginData->m_names.insert(filepath, json.value("name").toVariant());
    m_pluginData->m_versions.insert(filepath, json.value("version").toVariant());
    m_pluginData->m_dependencies.insert(filepath, json.value("dependencies").toArray().toVariantList());

    delete loader;
    loader = nullptr;
}

void PluginManager::setPluginList()
{
    m_configFile = PLUGIN_CONF_PATH;
    QFile file(m_configFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString strcons = QString("Failed to open config file:" + m_configFile);
        qDebug() << strcons;
        // LOG_WARN(strcons);
        //  qDebug() << strcons;
        return;
    }
    QString strcons = QString("读取配置文件成功:" + m_configFile);
    qDebug() << strcons;
    // LOG_INFO(strcons);
    //  qDebug() << strcons;

    QXmlStreamReader xml(&file);

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartDocument)
        {
            continue;
        }

        if (token == QXmlStreamReader::StartElement)
        {
            if (xml.name() == "plugin")
            {
                // Get the attributes of the plugin element
                QXmlStreamAttributes attributes = xml.attributes();
                QString              name       = attributes.value("name").toString();
                QString              path       = attributes.value("path").toString();
                QString              isUsed     = attributes.value("isUsed").toString();
#ifdef _DEBUG
                path = path + "d";
#else

#endif
                PluginCongInfo info;
                info.pluginName = name;
                info.pluginPath = path;
                info.isUsed     = isUsed;

                if (info.isUsed == "1")
                {
                    m_pluginData->pluginCongInfo.insert(info.pluginPath, info);
                    m_pluginData->m_loadOrder.append(info.pluginPath);
                }
            }
        }
    }
}

PluginManager::PluginManager()
{
    m_pluginData = new PluginManagerPrivate;
}

PluginManager::~PluginManager()
{
    if (m_pluginData)
    {
        delete m_pluginData;
        m_pluginData = nullptr;
    }
}
