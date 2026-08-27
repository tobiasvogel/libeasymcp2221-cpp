#include <cstddef>
#include <iostream>

#include <libeasymcp2221++/libeasymcp2221++.h>

using namespace libeasymcp2221;

int main()
{
    try {
        Device device;

        PinConfigurations configuration{};
        configuration[0] = {PinFunction::GpioInput, false};
        configuration[1] = {PinFunction::GpioOutput, true};
        configuration[2] = {PinFunction::Alt0, false};
        configuration[3] = {PinFunction::Dedicated, false};
        device.configurePins(configuration);

        const auto state = device.readGpio();
        for (std::size_t i = 0; i < state.pins.size(); ++i) {
            std::cout << "GP" << i << '=';
            if (state.pins[i].has_value()) {
                std::cout << (*state.pins[i] ? 1 : 0);
            }
            else {
                std::cout << "n/a";
            }
            std::cout << (i + 1 == state.pins.size() ? '\n' : ' ');
        }

        return 0;
    }
    catch (const Error& error) {
        std::cerr << "Pin configuration failed: " << error.what() << '\n';
        return 1;
    }
}
