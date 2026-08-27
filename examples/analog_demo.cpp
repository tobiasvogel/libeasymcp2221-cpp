#include <iostream>

#include <libeasymcp2221++/libeasymcp2221++.h>

using namespace libeasymcp2221;

int main()
{
    try {
        Device device;

        device.configureAdc(VoltageReference::Internal1V024);
        const auto adc = device.readAdcRaw();
        std::cout << "ADC raw: CH0(GP1)=" << adc[0]
                  << " CH1(GP2)=" << adc[1]
                  << " CH2(GP3)=" << adc[2] << '\n';

        device.configureDac(VoltageReference::Internal2V048, 16);
        device.writeDacRaw(8);
        std::cout << "DAC set to code 8 (ref 2.048V)\n";

        device.configureClock(ClockDutyCycle::Percent50, ClockFrequency::MHz12);
        std::cout << "Clock output set to 12MHz, 50% duty\n";
        return 0;
    }
    catch (const Error& error) {
        std::cerr << "Analog/clock example failed: " << error.what() << '\n';
        return 1;
    }
}
