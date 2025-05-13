#ifndef OPCUA_GLOBAL_H
#define OPCUA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(OPCUA_LIBRARY)
#define OPCUA_EXPORT Q_DECL_EXPORT
#else
#define OPCUA_EXPORT Q_DECL_IMPORT
#endif

#include "./opcuadefine.h"
#include "./leventbus.h"

#endif // LLOG_GLOBAL_H
