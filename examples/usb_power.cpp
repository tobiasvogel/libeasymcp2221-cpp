#include <iostream>

#include <libeasymcp2221++/libeasymcp2221++.h>

using namespace libeasymcp2221;

int main()
{
    try {
        Device device;

        device.stageUsbRemoteWakeup(true);
        device.stageUsbSelfPowered(false);
        device.stageUsbRequestedCurrent(100);

        // Persistent write, matching the C usb_power.c example.
        device.saveConfigurationToFlash();

        std::cout << "USB power configuration saved:\n"
                  << "  Remote Wake-up: enabled\n"
                  << "  Self-powered:   no\n"
                  << "  Bus current:    100 mA\n\n"
                  << "Reset or reconnect the MCP2221 for the new USB attributes "
                     "to take effect.\n";
        return 0;
    }
    catch (const Error& error) {
        std::cerr << "USB power configuration failed: " << error.what() << '\n';
        return 1;
    }
}
