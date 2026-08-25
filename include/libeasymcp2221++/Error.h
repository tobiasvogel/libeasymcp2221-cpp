/**
 * @file Error.h
 * @brief C++ exception and error-code types for libeasymcp2221++.
 */

#ifndef LIBEASYMCP2221_CPP_ERROR_H
#define LIBEASYMCP2221_CPP_ERROR_H

#include <stdexcept>
#include <string>

namespace libeasymcp2221 {

/** @brief Strongly typed counterpart of the libeasymcp2221 C error codes. */
enum class ErrorCode {
    Usb,
    Timeout,
    NotAcknowledged,
    LowScl,
    LowSda,
    Invalid,
    I2c,
    FlashWrite,
    FlashPassword,
    GpioMode,
    Generic,
    I2cShortRead,
    FlashRead,
    NotFound,
    NoMemory,
    Access,
    Busy,
    UsbInit,
    UsbEnumeration,
    UsbOpen,
    UsbClaim,
    CommandFailed,
    Protocol
};

/**
 * @brief Exception thrown when an underlying libeasymcp2221 operation fails.
 */
class Error : public std::runtime_error {
public:
    /**
     * @brief Construct an error.
     * @param code Typed error code.
     * @param nativeCode Original integer error code from the C library.
     * @param message Human-readable diagnostic message.
     */
    Error(ErrorCode code, int nativeCode, const std::string& message);

    /**
     * @brief Return the associated typed error code.
     * @return Error code supplied at construction.
     */
    ErrorCode code() const noexcept;

    /**
     * @brief Return the original integer error code from the C library.
     * @return Native mcp2221_error_code_t value converted to int.
     */
    int nativeCode() const noexcept;

private:
    ErrorCode code_;
    int nativeCode_;
};

} // namespace libeasymcp2221

#endif // LIBEASYMCP2221_CPP_ERROR_H
