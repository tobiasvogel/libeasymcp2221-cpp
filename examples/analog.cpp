/**
 * @file analog.cpp
 * @brief Example reading ADC channels and driving the DAC.
 */

#include <iostream>

#include <libeasymcp2221++/libeasymcp2221++.h>

int main()
{
    using namespace libeasymcp2221;

    try {
        Device device;

        // Supply the actual MCP2221 VDD when using VDD-referenced conversion.
        device.setVdd(3.3);
        device.configureAdc(VoltageReference::Vdd);

        const auto volts = device.readAdcVolts();

        std::cout << "ADC0: " << volts[0] << " V\n";
        std::cout << "ADC1: " << volts[1] << " V\n";
        std::cout << "ADC2: " << volts[2] << " V\n";

        // This changes the physical DAC output.
        device.configureDac(VoltageReference::Vdd);
        device.writeDacNormalized(0.5);

        return 0;
    }
    catch (const Error& error) {
        std::cerr << "MCP2221 error: " << error.what() << '\n';
        return 1;
    }
}
