#include <QDebug>
#include "leventbus.h"

struct CallbackInfo
{
    QObject *    obj;
    const char * slotName; // 槽函数名称

    // CallbackInfo(QObject * param1, const char * param2)
    //     : obj(param1)
    //     , slotName(param2){};
};
struct LEventBusPrivate
{
    std::map<QString, std::vector<CallbackInfo>> m_callbacks;  // 事件类型 -> 回调信息列表
    CircularQueue<QPair<QString, QVariant>>      m_eventQueue; // 事件队列
    std::mutex                                   m_mutex;
};

LEventBus & LEventBus::instance()
{
    static LEventBus instance;
    return instance;
}

bool processSlotName(const char *& slotName)
{
    // 判断字符串是否为空
    if (slotName == nullptr || slotName[0] == '\0')
    {
        return false;
    }

    // 检查第一个字符是否为 '1'
    if (slotName[0] == '1')
    {
        // 去除第一个字符
        slotName = slotName + 1;
        return true;
    }

    // 第一个字符不是 '1'，返回 false
    return false;
}

// 移除特定的 CallbackInfo 元素
bool removeSpecificCallbackInfo(std::vector<CallbackInfo> & vec, QObject * targetObj, const char * targetSlotName)
{
    // 使用 std::remove_if 查找并移除指定值的元素
    auto newEnd = std::remove_if(vec.begin(), vec.end(), [targetObj, targetSlotName](const CallbackInfo & info) {
        return info.obj == targetObj && std::string(info.slotName) == targetSlotName;
    });

    // 检查是否有元素被移动到末尾
    if (newEnd == vec.end())
    {
        return false; // 没有符合条件的元素
    }

    // 执行 erase 删除多余的元素
    vec.erase(newEnd, vec.end());
    return true; // 表示成功移除至少一个元素
}

void LEventBus::subscribe(const QString & event, const char * slotName, QObject * obj)
{
    std::lock_guard<std::mutex> lock(m_pData->m_mutex);
    if (!processSlotName(slotName))
    {
        return;
    }

    // 将事件与回调信息绑定，保存槽函数名和对象
    m_pData->m_callbacks[event].push_back({obj, slotName});
}

void LEventBus::publish(const QString & event, const QVariant & var)
{
    m_pData->m_eventQueue.enqueue(qMakePair(event, var));
}

void LEventBus::unSubscribe(const QString & event, const char * slotName, QObject * obj)
{
    std::lock_guard<std::mutex> lock(m_pData->m_mutex);
    if (!processSlotName(slotName))
    {
        return;
    }

    auto it = m_pData->m_callbacks.find(event);

    if (it != m_pData->m_callbacks.end())
    {
        if (it->second.size() <= 0)
        {
            return;
        }
        removeSpecificCallbackInfo(it->second, obj, slotName);
    }
}

void LEventBus::run()
{
    while (!isInterruptionRequested())
    {
        QPair<QString, QVariant> eventPair;
        if (m_pData->m_eventQueue.dequeue(eventPair))
        {
            std::lock_guard<std::mutex> lock(m_pData->m_mutex);
            auto                        it = m_pData->m_callbacks.find(eventPair.first);

            if (it != m_pData->m_callbacks.end())
            {
                // 发射事件，通过 invokeMethod 调用订阅者的槽
                for (const CallbackInfo & callbackInfo : it->second)
                {
                    // 动态调用每个订阅者的槽函数
                    QMetaObject::invokeMethod(callbackInfo.obj,
                                              callbackInfo.slotName, // 使用实际的槽函数名
                                              Qt::QueuedConnection,  // 在目标线程执行
                                              Q_ARG(const QVariant &, eventPair.second));
                }
            }
        }
    }
}

LEventBus::LEventBus()
    : m_pData(new LEventBusPrivate)
{
    start();
}

LEventBus::~LEventBus()
{
    m_pData->m_eventQueue.stop();
    requestInterruption();
    wait();
    if (m_pData)
    {
        delete m_pData;
        m_pData = nullptr;
    }
}