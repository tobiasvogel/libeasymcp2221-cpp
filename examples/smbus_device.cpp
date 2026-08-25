/**
 * @file smbus_device.cpp
 * @brief Example using the stateful SmbusDevice adapter.
 */

#include <cstdint>
#include <iomanip>
#include <iostream>

#include <libeasymcp2221++/libeasymcp2221++.h>

int main()
{
    try {
        libeasymcp2221::Device device;

        // Replace 0x2A with the 7-bit address of your SMBus target.
        auto target = device.smbusDevice(0x2A);

        // Replace command/register 0x00 for your device.
        const std::uint8_t value = target.readByteData(0x00);

        std::cout << "SMBus value: 0x"
                  << std::hex
                  << std::setw(2)
                  << std::setfill('0')
                  << static_cast<unsigned>(value)
                  << '\n';

        return 0;
    }
    catch (const libeasymcp2221::Error& error) {
        std::cerr << "MCP2221 error: " << error.what() << '\n';
        return 1;
    }
}
