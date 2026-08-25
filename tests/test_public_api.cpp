/**
 * @file test_public_api.cpp
 * @brief Hardware-free smoke tests for public C++ value types.
 */

#include <cstdint>
#include <type_traits>

#include <libeasymcp2221/Device.h>
#include <libeasymcp2221/DeviceOptions.h>
#include <libeasymcp2221/Error.h>
#include <libeasymcp2221/Types.h>

int main()
{
    using namespace libeasymcp2221;

    static_assert(!std::is_copy_constructible<Device>::value,
                  "Device must remain move-only");
    static_assert(std::is_move_constructible<Device>::value,
                  "Device must remain movable");

    DeviceOptions options;
    if (options.vendorId != 0x04D8u ||
        options.productId != 0x00DDu ||
        options.i2cSpeedHz != 100000u ||
        options.usbReadTimeoutMs != 500 ||
        options.commandRetries != 3) {
        return 1;
    }

    Error error(ErrorCode::Invalid, -6, "invalid");
    if (error.code() != ErrorCode::Invalid ||
        error.nativeCode() != -6) {
        return 2;
    }

    GpioWrite gpio;
    for (const auto& pin : gpio.pins) {
        if (pin.has_value()) {
            return 3;
        }
    }

    return 0;
}
