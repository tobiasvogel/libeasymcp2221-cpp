/**
 * @file ErrorInfo.h
 * @brief Qt-friendly error value type.
 */

#ifndef LIBEASYMCP2221_CPP_QT_ERROR_INFO_H
#define LIBEASYMCP2221_CPP_QT_ERROR_INFO_H

#include <QString>
#include <QMetaType>

#include <libeasymcp2221++/Error.h>

namespace libeasymcp2221::qt {

/**
 * @brief Value representation of an error crossing a Qt signal boundary.
 *
 * Core synchronous operations continue to throw libeasymcp2221::Error.
 * ErrorInfo is intended for asynchronous/event-driven Qt APIs where an
 * exception cannot be propagated directly to the caller.
 */
struct ErrorInfo {
    libeasymcp2221::ErrorCode code =
        libeasymcp2221::ErrorCode::Generic;

    int nativeCode = 0;
    QString message;
};

/**
 * @brief Convert a core library exception to its Qt value representation.
 */
[[nodiscard]] ErrorInfo toErrorInfo(
    const libeasymcp2221::Error& error);

}  // namespace libeasymcp2221::qt

Q_DECLARE_METATYPE(libeasymcp2221::qt::ErrorInfo)

#endif  // LIBEASYMCP2221_CPP_QT_ERROR_INFO_H