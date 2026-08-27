/**
 * @file QtTypes.h
 * @brief Qt meta-type declarations for libeasymcp2221++ value types.
 */

#ifndef LIBEASYMCP2221_CPP_QT_TYPES_H
#define LIBEASYMCP2221_CPP_QT_TYPES_H

#include <QMetaType>

#include <libeasymcp2221++/Error.h>
#include <libeasymcp2221++/Types.h>

Q_DECLARE_METATYPE(libeasymcp2221::Pin)
Q_DECLARE_METATYPE(libeasymcp2221::GpioEdge)
Q_DECLARE_METATYPE(libeasymcp2221::GpioEvent)
Q_DECLARE_METATYPE(libeasymcp2221::ErrorCode)

#endif  // LIBEASYMCP2221_CPP_QT_TYPES_H