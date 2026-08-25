/**
 * @file gpio.cpp
 * @brief Example configuring and reading MCP2221 GPIO pins.
 */

#include <iostream>

#include <libeasymcp2221++/libeasymcp2221++.h>

int main()
{
    using namespace libeasymcp2221;

    try {
        Device device;

        PinConfigurations pins;
        pins[0].function = PinFunction::GpioOutput;
        pins[0].outputValue = false;

        pins[1].function = PinFunction::GpioInput;

        device.configurePins(pins);

        GpioWrite update;
        update.pins[0] = true;
        device.writeGpio(update);

        const GpioState state = device.readGpio();

        if (state.pins[1].has_value()) {
            std::cout << "GP1 = " << (*state.pins[1] ? "high" : "low") << '\n';
        }

        return 0;
    }
    catch (const Error& error) {
        std::cerr << "MCP2221 error: " << error.what() << '\n';
        return 1;
    }
}
