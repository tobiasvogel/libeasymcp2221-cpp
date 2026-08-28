/**
 * @file MockControl.h
 * @brief Test-only control and inspection API for the mock C backend.
 */

#ifndef LIBEASYMCP2221_CPP_TEST_MOCK_CONTROL_H
#define LIBEASYMCP2221_CPP_TEST_MOCK_CONTROL_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

extern "C" {
#include <libeasymcp2221/mcp2221_error_codes.h>
#include <libeasymcp2221/mcp2221_gpio_poll.h>
#include <libeasymcp2221/mcp2221_i2c_slave.h>
}

namespace libeasymcp2221_test {

void resetMock();
void failNext(mcp2221_error_code_t error);

int openCount();
int closeCount();
int usbCurrentSetCount();
std::size_t lastFlashWriteSize();

int lastClockDuty();
std::string lastClockFrequency();

int lastGpio0();
int lastGpio1();
int lastGpio2();
int lastGpio3();
std::uint16_t lastGpioFilterMask();

const std::vector<std::uint8_t>& lastI2cWriteData();
std::uint32_t lastI2cRegister();
int lastI2cRegisterWidth();
mcp2221_i2c_byte_order_t lastI2cByteOrder();

std::uint8_t lastSmbusCommand();
const std::vector<std::uint8_t>& lastSmbusWriteData();

void queueGpioEvent(
    std::uint8_t gpio,
    mcp2221_gpio_event_type_t type,
    double time = 2.0,
    double lastTime = 1.0);

} // namespace libeasymcp2221_test

#endif
