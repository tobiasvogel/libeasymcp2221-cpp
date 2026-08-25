/**
 * @file i2c_device.cpp
 * @brief Example using the stateful I2cDevice adapter.
 */

#include <cstdint>
#include <iomanip>
#include <iostream>

#include <libeasymcp2221++/libeasymcp2221++.h>

int main()
{
    try {
        libeasymcp2221::Device device;

        // Replace 0x50 with the 7-bit address of your target.
        auto target = device.i2cDevice(0x50);

        if (!target.isPresent()) {
            std::cerr << "I2C target did not acknowledge\n";
            return 2;
        }

        // Replace register 0x00 and the read length for your device.
        const auto data = target.readRegister(0x00, 2);

        std::cout << "Read:";
        for (std::uint8_t byte : data) {
            std::cout << " 0x"
                      << std::hex
                      << std::setw(2)
                      << std::setfill('0')
                      << static_cast<unsigned>(byte);
        }
        std::cout << '\n';

        return 0;
    }
    catch (const libeasymcp2221::Error& error) {
        std::cerr << "MCP2221 error: " << error.what() << '\n';
        return 1;
    }
}
