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
    /** @brief Strongly typed libeasymcp2221++ error code. */
    libeasymcp2221::ErrorCode code =
        libeasymcp2221::ErrorCode::Generic;

    /** @brief Original integer error code reported by the C library. */
    int nativeCode = 0;

    /** @brief Human-readable error description. */
    QString message;
};

/**
 * @brief Convert a core library exception to its Qt value representation.
 * @param error Core libeasymcp2221++ exception to convert.
 * @return Qt-friendly error information carrying code, native code, and message.
 */
[[nodiscard]] ErrorInfo toErrorInfo(
    const libeasymcp2221::Error& error);

}  // namespace libeasymcp2221::qt

Q_DECLARE_METATYPE(libeasymcp2221::qt::ErrorInfo)

#endif  // LIBEASYMCP2221_CPP_QT_ERROR_INFO_H