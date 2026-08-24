/**
 * @file Error.cpp
 * @brief Error and C error-code translation implementation.
 */

#include "libeasymcp2221/Error.h"

#include <string>

extern "C" {
#include <libeasymcp2221/mcp2221_errors.h>
}

#include "detail/CheckError.h"

namespace libeasymcp2221 {

Error::Error(ErrorCode code, int nativeCode, const std::string& message)
    : std::runtime_error(message),
      code_(code),
      nativeCode_(nativeCode)
{
}

ErrorCode Error::code() const noexcept
{
    return code_;
}

int Error::nativeCode() const noexcept
{
    return nativeCode_;
}

namespace detail {

ErrorCode toErrorCode(mcp2221_error_code_t code)
{
    switch (code) {
    case MCP2221_ERR_USB: return ErrorCode::Usb;
    case MCP2221_ERR_TIMEOUT: return ErrorCode::Timeout;
    case MCP2221_ERR_NOT_ACK: return ErrorCode::NotAcknowledged;
    case MCP2221_ERR_LOW_SCL: return ErrorCode::LowScl;
    case MCP2221_ERR_LOW_SDA: return ErrorCode::LowSda;
    case MCP2221_ERR_INVALID: return ErrorCode::Invalid;
    case MCP2221_ERR_I2C: return ErrorCode::I2c;
    case MCP2221_ERR_FLASH_WRITE: return ErrorCode::FlashWrite;
    case MCP2221_ERR_FLASH_PASSWD: return ErrorCode::FlashPassword;
    case MCP2221_ERR_GPIO_MODE: return ErrorCode::GpioMode;
    case MCP2221_ERR_I2C_SHORT_READ: return ErrorCode::I2cShortRead;
    case MCP2221_ERR_FLASH_READ: return ErrorCode::FlashRead;
    case MCP2221_ERR_NOT_FOUND: return ErrorCode::NotFound;
    case MCP2221_ERR_NO_MEMORY: return ErrorCode::NoMemory;
    case MCP2221_ERR_ACCESS: return ErrorCode::Access;
    case MCP2221_ERR_BUSY: return ErrorCode::Busy;
    case MCP2221_ERR_USB_INIT: return ErrorCode::UsbInit;
    case MCP2221_ERR_USB_ENUM: return ErrorCode::UsbEnumeration;
    case MCP2221_ERR_USB_OPEN: return ErrorCode::UsbOpen;
    case MCP2221_ERR_USB_CLAIM: return ErrorCode::UsbClaim;
    case MCP2221_ERR_COMMAND_FAILED: return ErrorCode::CommandFailed;
    case MCP2221_ERR_PROTOCOL: return ErrorCode::Protocol;
    case MCP2221_ERR_GENERIC:
    case MCP2221_ERR_OK:
    default:
        return ErrorCode::Generic;
    }
}

void checkError(mcp2221_error_code_t code, const char* operation)
{
    if (code == MCP2221_ERR_OK) {
        return;
    }

    std::string message;
    if (operation != nullptr && operation[0] != '\0') {
        message += operation;
        message += ": ";
    }
    message += mcp2221_error_code_to_string(code);

    throw Error(toErrorCode(code), static_cast<int>(code), message);
}

[[noreturn]] void throwInvalid(const char* message)
{
    throw Error(
        ErrorCode::Invalid,
        static_cast<int>(MCP2221_ERR_INVALID),
        message != nullptr ? message : "Invalid argument");
}

} // namespace detail
} // namespace libeasymcp2221
