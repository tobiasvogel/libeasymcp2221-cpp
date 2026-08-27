/**
 * @file test_core.cpp
 * @brief Hardware-free behavioral tests for libeasymcp2221++.
 */

#include <cstdint>
#include <string>
#include <vector>

#include <libeasymcp2221++/libeasymcp2221++.h>

extern "C" {
#include <libeasymcp2221/mcp2221_gpio.h>
#include <libeasymcp2221/mcp2221_gpio_poll.h>
}

#include "MockControl.h"
#include "TestHarness.h"

using namespace libeasymcp2221;
using namespace libeasymcp2221_test;

namespace {

void testErrorMapping()
{
    resetMock();

    Device device;
    failNext(MCP2221_ERR_TIMEOUT);

    bool caught = false;
    try {
        device.setI2cSpeed(100000);
    }
    catch (const Error& error) {
        caught = true;
        EXPECT_EQ(error.code(), ErrorCode::Timeout);
        EXPECT_EQ(error.nativeCode(), static_cast<int>(MCP2221_ERR_TIMEOUT));
        EXPECT_TRUE(
            std::string(error.what()).find("Setting I2C speed") !=
            std::string::npos);
    }

    EXPECT_TRUE(caught);
}

void testDeviceChildLifetime()
{
    resetMock();

    {
        Device device;
        EXPECT_EQ(openCount(), 1);
        EXPECT_EQ(closeCount(), 0);

        {
            auto target = device.i2cDevice(0x50);
            device.close();

            EXPECT_FALSE(device.isOpen());
            EXPECT_EQ(closeCount(), 0);

            const auto data = target.read(1);
            EXPECT_EQ(data.size(), static_cast<std::size_t>(1));
            EXPECT_EQ(data[0], static_cast<std::uint8_t>(0xA5));
        }

        EXPECT_EQ(closeCount(), 1);
    }

    EXPECT_EQ(closeCount(), 1);
}

void testRawI2cAndI2cDevice()
{
    resetMock();

    Device device;

    const auto raw = device.i2cRead(0x22, 3);
    EXPECT_EQ(raw.size(), static_cast<std::size_t>(3));
    EXPECT_EQ(raw[0], static_cast<std::uint8_t>(0x5A));

    auto target = device.i2cDevice(0x50);
    EXPECT_EQ(target.address(), static_cast<std::uint8_t>(0x50));
    EXPECT_TRUE(target.isPresent());

    const auto direct = target.read(2);
    EXPECT_EQ(direct.size(), static_cast<std::size_t>(2));
    EXPECT_EQ(direct[0], static_cast<std::uint8_t>(0xA5));

    const auto reg = target.readRegister(0x1234, 2, RegisterWidth::Bits16, ByteOrder::BigEndian);
    EXPECT_EQ(reg[0], static_cast<std::uint8_t>(0x3C));

    bool caught = false;
    try {
        static_cast<void>(target.read(0));
    }
    catch (const Error& error) {
        caught = true;
        EXPECT_EQ(error.code(), ErrorCode::Invalid);
    }
    EXPECT_TRUE(caught);
}

void testSmbusDevice()
{
    resetMock();

    Device device;
    auto target = device.smbusDevice(0x2A);

    EXPECT_EQ(target.address(), static_cast<std::uint8_t>(0x2A));
    EXPECT_EQ(target.readByte(), static_cast<std::uint8_t>(0x12));
    EXPECT_EQ(
        target.readByteData(0x01),
        static_cast<std::uint8_t>(0x34));
    EXPECT_EQ(
        target.readWordData(0x02),
        static_cast<std::int16_t>(0x1234));
    EXPECT_EQ(
        target.processCall(0x03, 0x4321),
        static_cast<std::int16_t>(0x4321));

    const auto block = target.readBlockData(0x04);
    EXPECT_EQ(block.size(), static_cast<std::size_t>(2));
    EXPECT_EQ(block[0], static_cast<std::uint8_t>(1));
    EXPECT_EQ(block[1], static_cast<std::uint8_t>(2));

    const auto i2cBlock = target.readI2cBlockData(0x05, 3);
    EXPECT_EQ(i2cBlock.size(), static_cast<std::size_t>(3));
    EXPECT_EQ(i2cBlock[0], static_cast<std::uint8_t>(0x66));
}

void testGpioOptionalTranslation()
{
    resetMock();

    Device device;

    GpioWrite values;
    values.pins[0] = true;
    values.pins[2] = false;

    device.writeGpio(values);

    EXPECT_EQ(lastGpio0(), 1);
    EXPECT_EQ(lastGpio1(), MCP2221_GPIO_KEEP);
    EXPECT_EQ(lastGpio2(), 0);
    EXPECT_EQ(lastGpio3(), MCP2221_GPIO_KEEP);

    const auto state = device.readGpio();
    EXPECT_TRUE(state.pins[0].has_value());
    EXPECT_FALSE(*state.pins[0]);
    EXPECT_TRUE(state.pins[1].has_value());
    EXPECT_TRUE(*state.pins[1]);
    EXPECT_FALSE(state.pins[2].has_value());
    EXPECT_FALSE(state.pins[3].has_value());
}


void testTypedGpioEventFilter()
{
    resetMock();

    Device device;
    auto poller = device.gpioPoller();

    GpioEventFilter filter;
    filter.rising[0] = true;
    filter.falling[2] = true;
    poller.setFilter(filter);

    EXPECT_EQ(
        lastGpioFilterMask(),
        static_cast<std::uint16_t>(
            MCP2221_GPIO_POLL_MASK_RISE(0) |
            MCP2221_GPIO_POLL_MASK_FALL(2)));

    poller.clearFilter();
    EXPECT_EQ(lastGpioFilterMask(), static_cast<std::uint16_t>(0));
}

void testGpioEventId()
{
    GpioEvent event;
    event.pin = Pin::GP0;
    event.edge = GpioEdge::Rising;
    EXPECT_EQ(event.id(), std::string("GPIO0_RISE"));

    event.pin = Pin::GP3;
    event.edge = GpioEdge::Falling;
    EXPECT_EQ(event.id(), std::string("GPIO3_FALL"));
}

void testAnalogClockAndUsb()
{
    resetMock();

    Device device;

    device.setVdd(3.3);
    EXPECT_EQ(device.vdd(), 3.3);

    const auto raw = device.readAdcRaw();
    EXPECT_EQ(raw[0], static_cast<std::uint16_t>(1));
    EXPECT_EQ(raw[1], static_cast<std::uint16_t>(2));
    EXPECT_EQ(raw[2], static_cast<std::uint16_t>(3));

    device.configureClock(
        ClockDutyCycle::Percent50,
        ClockFrequency::MHz6);

    EXPECT_EQ(lastClockDuty(), 50);
    EXPECT_EQ(lastClockFrequency(), std::string("6MHz"));

    bool caught = false;
    try {
        device.stageUsbRequestedCurrent(101);
    }
    catch (const Error& error) {
        caught = true;
        EXPECT_EQ(error.code(), ErrorCode::Invalid);
    }

    EXPECT_TRUE(caught);
    EXPECT_EQ(usbCurrentSetCount(), 0);

    device.stageUsbRequestedCurrent(100);
    EXPECT_EQ(usbCurrentSetCount(), 1);
    EXPECT_EQ(device.usbRequestedCurrent(), 100u);
}

void testExtendedFlashWrite()
{
    resetMock();

    Device device;
    const std::vector<std::uint8_t> data(62);
    device.writeFlash(FlashSection::UsbProduct, data);

    EXPECT_EQ(lastFlashWriteSize(), data.size());
}

void testInvalidI2cAddress()
{
    resetMock();

    Device device;

    bool caught = false;
    try {
        static_cast<void>(device.i2cRead(0x80, 1));
    }
    catch (const Error& error) {
        caught = true;
        EXPECT_EQ(error.code(), ErrorCode::Invalid);
    }

    EXPECT_TRUE(caught);
}

} // namespace

int main()
{
    int failures = 0;

    test_harness::run(
        "Error mapping preserves typed and native code",
        testErrorMapping,
        failures);

    test_harness::run(
        "Child adapter keeps shared device state alive",
        testDeviceChildLifetime,
        failures);

    test_harness::run(
        "Raw I2C and I2cDevice behavior",
        testRawI2cAndI2cDevice,
        failures);

    test_harness::run(
        "SMBus adapter behavior",
        testSmbusDevice,
        failures);

    test_harness::run(
        "GPIO optional values map to C sentinels",
        testGpioOptionalTranslation,
        failures);

    test_harness::run(
        "Typed GPIO event filter maps to native mask",
        testTypedGpioEventFilter,
        failures);

    test_harness::run(
        "GPIO event compatibility identifiers",
        testGpioEventId,
        failures);

    test_harness::run(
        "Analog, clock, and USB value translation",
        testAnalogClockAndUsb,
        failures);

    test_harness::run(
        "Extended flash writes preserve the complete payload",
        testExtendedFlashWrite,
        failures);

    test_harness::run(
        "Invalid I2C addresses fail before the C call",
        testInvalidI2cAddress,
        failures);

    return failures == 0 ? 0 : 1;
}
