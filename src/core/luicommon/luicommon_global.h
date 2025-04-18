#ifndef LUICOMMON_GLOBAL_H
#define LUICOMMON_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LUICOMMON_LIBRARY)
#define LUICOMMON_EXPORT Q_DECL_EXPORT
#else
#define LUICOMMON_EXPORT Q_DECL_IMPORT
#endif

#endif // LLOG_GLOBAL_H
