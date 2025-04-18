#ifndef LCORE_GLOBAL_H
#define LCORE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LCORE_LIBRARY)
#define LCORE_EXPORT Q_DECL_EXPORT
#else
#define LCORE_EXPORT Q_DECL_IMPORT
#endif

#endif // LLOG_GLOBAL_H
