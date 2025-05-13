#ifndef LEVENTBUS_H
#define LEVENTBUS_H

#include <map>
#include <vector>
#include <functional>
#include <mutex>
#include <thread>
#include <QVariant> // 引入 QVariant
#include <QString>  // 引入 QString
#include <iostream>
#include <QThread>
#include <memory>

#include "circularqueue.h"
#include "opcua_global.h"

struct LEventBusPrivate;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	消息队列类,支持多线程. </summary>
///
/// <remarks>	Liruilong, 2024/10/25. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
class OPCUA_EXPORT LEventBus : public QThread
{
    Q_OBJECT
public:
    static LEventBus & instance();

    // 订阅
    void subscribe(const QString & event, const char * slotName, QObject * obj);

    // 发布
    void publish(const QString & event, const QVariant & var);

    void unSubscribe(const QString & event, const char * slotName, QObject * obj);

    void run() override;

private:
    LEventBus();
    ~LEventBus();

    LEventBusPrivate * m_pData{nullptr};
};

// 简化使用方法
inline void eventSubscribe(const QString & event, const char * slotName, QObject * obj)
{
    LEventBus::instance().subscribe(event, slotName, obj);
}

inline void eventUnSubscribe(const QString & event, const char * slotName, QObject * obj)
{
    LEventBus::instance().unSubscribe(event, slotName, obj);
}

inline void eventPublish(const QString & event, const QVariant & var)
{
    LEventBus::instance().publish(event, var);
}

#endif // LEVENTBUS_H