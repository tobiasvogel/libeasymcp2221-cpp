/**
 * @file sram.cpp
 * @brief Example applying a partial runtime SRAM configuration.
 */

#include <libeasymcp2221++/libeasymcp2221++.h>

#include <iostream>

int main() {
	using namespace libeasymcp2221;

	try {
		Device device;

		SramConfig config;

		config.gpio[0].function = SramPinFunction::Gpio;
		config.gpio[0].direction = GpioDirection::Output;
		config.gpio[0].outputValue = false;

		config.adc.reference = VoltageReference::Vdd;
		config.clock.frequency = ClockFrequency::MHz6;
		config.clock.dutyCycle = ClockDutyCycle::Percent50;

		// Runtime SRAM only: this does not persist to flash.
		device.configureSram(config);

		std::cout << "Runtime SRAM configuration applied\n";
		return 0;
	} catch (const Error& error) {
		std::cerr << "MCP2221 error: " << error.what() << '\n';
		return 1;
	}
}
