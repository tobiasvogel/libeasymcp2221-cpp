#include <cstdint>
#include <iomanip>
#include <iostream>

#include <libeasymcp2221++/libeasymcp2221++.h>

using namespace libeasymcp2221;

int main()
{
    try {
        Device device;

        I2cDeviceOptions options{};
        options.force = true;
        options.speedHz = 100000;
        options.registerWidth = RegisterWidth::Bits16;
        options.byteOrder = ByteOrder::BigEndian;

        auto eeprom = device.i2cDevice(0x50, options);
        const auto data = eeprom.readRegister(0x0000, 16);

        std::cout << "EEPROM[0..15]:";
        for (const auto byte : data) {
            std::cout << ' ' << std::uppercase << std::hex
                      << std::setw(2) << std::setfill('0')
                      << static_cast<unsigned>(byte);
        }
        std::cout << std::dec << '\n';

        return 0;
    }
    catch (const Error& error) {
        std::cerr << "EEPROM read failed: " << error.what() << '\n';
        return 1;
    }
}
