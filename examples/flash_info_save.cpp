#include <iostream>

#include <libeasymcp2221++/libeasymcp2221++.h>

using namespace libeasymcp2221;

int main()
{
    try {
        Device device;

        const auto info = device.flashInfo();
        std::cout << "USB Manufacturer: " << info.usbManufacturer << '\n'
                  << "USB Product     : " << info.usbProduct << '\n'
                  << "USB Serial      : " << info.usbSerial << '\n'
                  << "Factory Serial  : " << info.factorySerial << '\n';

        std::cout << "\nWriting current SRAM settings back to flash "
                     "(like the C example). This is persistent.\n";
        device.saveConfigurationToFlash();
        std::cout << "Done.\n";

        return 0;
    }
    catch (const Error& error) {
        std::cerr << "Flash info/save failed: " << error.what() << '\n';
        return 1;
    }
}
