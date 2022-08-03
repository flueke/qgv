#ifndef QGV_EXPORT_H
#define QGV_EXPORT_H

#include <QtGlobal>

#ifdef QGVCORE_LIB
	#define QGVCORE_EXPORT Q_DECL_EXPORT
#else
	#define QGVCORE_EXPORT Q_DECL_IMPORT
#endif

#endif // QGV_EXPORT_H
