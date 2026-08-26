/**
 * @file Constants.h
 * @brief Public constants used by libeasymcp2221++.
 */

#ifndef LIBEASYMCP2221_CPP_CONSTANTS_H
#define LIBEASYMCP2221_CPP_CONSTANTS_H

#include <cstddef>
#include <cstdint>

namespace libeasymcp2221 {
namespace constants {

/** @brief Default Microchip USB vendor ID for MCP2221 devices. */
constexpr std::uint16_t DefaultVendorId = 0x04D8;

/** @brief Default USB product ID for MCP2221/MCP2221A devices. */
constexpr std::uint16_t DefaultProductId = 0x00DD;

/** @brief MCP2221 HID report size in bytes. */
constexpr std::size_t PacketSize = 64;

/** @brief Largest valid 7-bit I2C address. */
constexpr std::uint8_t MaxI2cAddress = 0x7F;

/** @brief Maximum supported I2C transfer size in bytes. */
constexpr std::size_t MaxI2cTransfer = 65535;

/** @brief Maximum supported MCP2221 I2C bus clock in hertz. */
constexpr std::uint32_t MaxI2cSpeedHz = 400000;

/** @brief Maximum USB requested current in milliamperes. */
constexpr unsigned MaxUsbCurrentMa = 500;

/** @brief Minimum supported externally supplied VDD value in volts. */
constexpr double MinVddVolts = 3.0;

/** @brief Maximum supported externally supplied VDD value in volts. */
constexpr double MaxVddVolts = 5.5;

}  // namespace constants
}  // namespace libeasymcp2221

#endif	// LIBEASYMCP2221_CPP_CONSTANTS_H
