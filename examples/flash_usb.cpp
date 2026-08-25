/**
 * @file flash_usb.cpp
 * @brief Example reading flash information and staging USB configuration.
 *
 * @warning This example can perform persistent writes when
 *          saveConfigurationToFlash() is enabled below.
 */

#include <iostream>

#include <libeasymcp2221++/libeasymcp2221++.h>

int main()
{
    using namespace libeasymcp2221;

    try {
        Device device;

        const FlashInfo info = device.flashInfo();

        std::cout << "Manufacturer: " << info.usbManufacturer << '\n';
        std::cout << "Product:      " << info.usbProduct << '\n';
        std::cout << "USB serial:   " << info.usbSerial << '\n';

        // These calls stage values in the Device context only.
        device.stageUsbRemoteWakeup(false);
        device.stageUsbSelfPowered(false);
        device.stageUsbRequestedCurrent(100);

        std::cout << "USB settings staged but NOT persisted\n";

        /*
         * Persistent write:
         *
         * Uncomment only when you intentionally want to modify the MCP2221
         * flash configuration. The operation is not atomic, and the USB host
         * observes changed enumeration settings only after re-enumeration.
         */
        // device.saveConfigurationToFlash();

        return 0;
    }
    catch (const Error& error) {
        std::cerr << "MCP2221 error: " << error.what() << '\n';
        return 1;
    }
}
