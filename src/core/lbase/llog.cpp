#include <string.h>
#include <iostream>
#include <QDateTime>
#include <QDebug>
#include <core/spdlog/spdlog.h>
#include <core/spdlog/sinks/basic_file_sink.h>
#include <core/spdlog/sinks/stdout_color_sinks.h>
#include <core/spdlog/sinks/rotating_file_sink.h>

#include "llog.h"

#define CURRENT_TIME QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")

struct LLogPrivate
{
    bool                            isSave{true}; // 是否保存
    std::shared_ptr<spdlog::logger> mylogger;     // 日志保存到文件中
    // std::shared_ptr<spdlog::logger> consoleLogger; // 日志显示到控制台
};

void LLog::saveLog(bool isSave)
{
    if (m_logData)
    {
        m_logData->isSave = isSave;
    }
}

void LLog::printWaringStd(std::string msg)
{
    if (m_logData && m_logData->mylogger)
    {
        m_logData->mylogger->warn(msg);
        m_logData->mylogger->flush();
    }
}

void LLog::printWaring(QString msg)
{
    qDebug().noquote() << QString("[%1] (Warning): %2").arg(CURRENT_TIME).arg(msg);
    if (m_logData && m_logData->isSave)
        printWaringStd(msg.toStdString());
}

void LLog::printDebugStd(std::string msg)
{
    if (m_logData && m_logData->mylogger)
    {
        m_logData->mylogger->debug(msg);
        m_logData->mylogger->flush();
    }
}

void LLog::printDebug(QString msg)
{
    qDebug().noquote() << QString("[%1] (Debug): %2").arg(CURRENT_TIME).arg(msg);
    if (m_logData && m_logData->isSave)
        printDebugStd(msg.toStdString());
}

void LLog::printErrorStd(std::string msg)
{
    if (m_logData && m_logData->mylogger)
    {
        m_logData->mylogger->error(msg);
        m_logData->mylogger->flush();
    }
}

void LLog::printError(QString msg)
{
    qDebug().noquote() << QString("[%1] (Error): %2").arg(CURRENT_TIME).arg(msg);
    if (m_logData && m_logData->isSave)
        printErrorStd(msg.toStdString());
}

void LLog::printInfoStd(std::string msg)
{
    if (m_logData && m_logData->mylogger)
    {
        m_logData->mylogger->info(msg);
        m_logData->mylogger->flush();
    }
}

void LLog::printInfo(QString msg)
{
    qDebug().noquote() << QString("[%1] (Info): %2").arg(CURRENT_TIME).arg(msg);
    if (m_logData && m_logData->isSave)
    {
        printInfoStd(msg.toStdString());
    }
}

LLog::LLog()
{
    // 文件名（当前时间）
    QDateTime   curTime     = QDateTime::currentDateTime();
    QString     fileName    = curTime.toString("yyyy_MM_dd_hh_mm_ss");
    std::string logFileName = "./log/" + fileName.toStdString() + ".log";

    m_logData           = new LLogPrivate;
    m_logData->mylogger = spdlog::basic_logger_mt("app", logFileName);

    // m_logData->consoleLogger = spdlog::stdout_color_mt("console");
    // m_logData->consoleLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e](%l): %v");

    m_logData->mylogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e](%l): %v");
    m_logData->mylogger->set_level(spdlog::level::debug);
    spdlog::flush_every(std::chrono::seconds(3)); // 定期刷新日志缓冲区
}

LLog::~LLog()
{
    if (m_logData)
    {
        delete m_logData;
        m_logData = nullptr;
    }
}
