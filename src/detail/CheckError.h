/**
 * @file CheckError.h
 * @brief Internal C-to-C++ error translation helpers.
 */

#ifndef LIBEASYMCP2221_CPP_DETAIL_CHECK_ERROR_H
#define LIBEASYMCP2221_CPP_DETAIL_CHECK_ERROR_H

#include "libeasymcp2221++/Error.h"

extern "C" {
#include <libeasymcp2221/mcp2221_error_codes.h>
}

namespace libeasymcp2221 {
namespace detail {

/**
 * @brief Throw Error when @p code is not MCP2221_ERR_OK.
 * @param code Native C error code.
 * @param operation Optional operation name included in the exception message.
 */
void checkError(mcp2221_error_code_t code, const char* operation = nullptr);

/** @brief Convert a C error code to the public C++ ErrorCode enum. */
ErrorCode toErrorCode(mcp2221_error_code_t code);

/**
 * @brief Throw a public Invalid error for a C++-side validation failure.
 * @param message Human-readable validation failure.
 */
[[noreturn]] void throwInvalid(const char* message);

} // namespace detail
} // namespace libeasymcp2221

#endif // LIBEASYMCP2221_CPP_DETAIL_CHECK_ERROR_H
