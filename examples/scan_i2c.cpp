#include <cstdint>
#include <iomanip>
#include <iostream>

#include <libeasymcp2221++/libeasymcp2221++.h>

using namespace libeasymcp2221;

int main()
{
    try {
        Device device;
        device.setI2cSpeed(100000);

        std::cout << "Scanning I2C bus using MCP2221 at 100kHz...\n";
        std::cout << "    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n";
        std::cout << "00: ";

        for (std::uint16_t address = 0; address < 128; ++address) {
            if (address != 0 && address % 16 == 0) {
                std::cout << '\n' << std::uppercase << std::hex
                          << std::setw(2) << std::setfill('0') << address << ": ";
            }

            auto target = device.i2cDevice(static_cast<std::uint8_t>(address));
            if (target.isPresent()) {
                std::cout << std::uppercase << std::hex
                          << std::setw(2) << std::setfill('0') << address << ' ';
            }
            else {
                std::cout << "-- ";
            }
        }

        std::cout << std::dec << "\nDone.\n";
        return 0;
    }
    catch (const Error& error) {
        std::cerr << "I2C scan failed: " << error.what() << '\n';
        return 1;
    }
}
