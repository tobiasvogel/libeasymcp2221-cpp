/**
 * @file DeviceOptions.h
 * @brief Device-opening options for libeasymcp2221++.
 */

#ifndef LIBEASYMCP2221_CPP_DEVICE_OPTIONS_H
#define LIBEASYMCP2221_CPP_DEVICE_OPTIONS_H

#include <cstdint>
#include <string>

#include "Constants.h"

namespace libeasymcp2221 {

/**
 * @brief Options used when opening an MCP2221(A).
 *
 * The defaults correspond to the standard Microchip VID/PID and a 100 kHz
 * initial I2C bus speed.
 */
struct DeviceOptions {
    std::uint16_t vendorId = constants::DefaultVendorId;
    std::uint16_t productId = constants::DefaultProductId;
    int deviceIndex = 0;
    std::string usbSerial;

    int usbReadTimeoutMs = 500;
    int commandRetries = 3;
    bool debugMessages = false;
    bool tracePackets = false;
    bool scanFlashSerial = false;

    std::uint32_t i2cSpeedHz = 100000;
};

} // namespace libeasymcp2221

#endif // LIBEASYMCP2221_CPP_DEVICE_OPTIONS_H
