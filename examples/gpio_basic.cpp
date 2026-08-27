#include <cstddef>
#include <iostream>

#include <libeasymcp2221++/libeasymcp2221++.h>

using namespace libeasymcp2221;

namespace {

void dumpStates(const GpioState& state)
{
    for (std::size_t i = 0; i < state.pins.size(); ++i) {
        std::cout << "GP" << i << ": ";
        if (!state.pins[i].has_value()) {
            std::cout << "mode=ALT value=n/a\n";
        }
        else {
            std::cout << "mode=GPIO value=" << (*state.pins[i] ? "HIGH" : "LOW") << '\n';
        }
    }
}

}  // namespace

int main()
{
    try {
        Device device;

        PinConfigurations configuration{};
        configuration[0] = {PinFunction::GpioOutput, true};
        configuration[1] = {PinFunction::GpioInput, false};
        configuration[2] = {PinFunction::GpioOutput, false};
        configuration[3] = {PinFunction::GpioInput, false};
        device.configurePins(configuration);

        GpioWrite write{};
        write.pins[0] = true;
        write.pins[2] = false;
        device.writeGpio(write);

        dumpStates(device.readGpio());
        return 0;
    }
    catch (const Error& error) {
        std::cerr << "GPIO example failed: " << error.what() << '\n';
        return 1;
    }
}
