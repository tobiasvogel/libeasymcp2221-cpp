/**
 * @file MockControl.h
 * @brief Test-only control and inspection API for the mock C backend.
 */

#ifndef LIBEASYMCP2221_CPP_TEST_MOCK_CONTROL_H
#define LIBEASYMCP2221_CPP_TEST_MOCK_CONTROL_H

#include <cstdint>
#include <string>

extern "C" {
#include <libeasymcp2221/mcp2221_error_codes.h>
}

namespace libeasymcp2221_test {

void resetMock();
void failNext(mcp2221_error_code_t error);

int openCount();
int closeCount();
int usbCurrentSetCount();

int lastClockDuty();
std::string lastClockFrequency();

int lastGpio0();
int lastGpio1();
int lastGpio2();
int lastGpio3();
std::uint16_t lastGpioFilterMask();

} // namespace libeasymcp2221_test

#endif
