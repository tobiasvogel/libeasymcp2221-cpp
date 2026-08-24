/**
 * @file Device.cpp
 * @brief Device implementation and initial API stubs.
 */

#include "libeasymcp2221/Device.h"

#include <stdexcept>
#include <utility>

#include "libeasymcp2221/GpioPoller.h"
#include "libeasymcp2221/I2cDevice.h"
#include "libeasymcp2221/SmbusDevice.h"
#include "detail/CheckError.h"
#include "detail/DeviceState.h"

namespace libeasymcp2221 {

namespace {

[[noreturn]] void notImplemented(const char* operation)
{
    throw std::logic_error(std::string(operation) + " is not implemented yet");
}

} // namespace

Device::Device()
    : Device(DeviceOptions{})
{
}

Device::Device(const DeviceOptions& options)
{
    mcp2221_t* handle = nullptr;

    const char* serial =
        options.usbSerial.empty() ? nullptr : options.usbSerial.c_str();

    const auto result = mcp2221_open_simple_scan(
        options.vendorId,
        options.productId,
        options.deviceIndex,
        serial,
        static_cast<int>(options.i2cSpeedHz),
        options.scanFlashSerial ? 1 : 0,
        &handle);

    detail::checkError(result);
    state_ = std::make_shared<detail::DeviceState>(handle);

    /*
     * TODO: Decide whether non-simple transport options
     * (usbReadTimeoutMs, commandRetries, debugMessages, tracePackets)
     * should cause this constructor to use mcp2221_open_scan() followed by
     * explicit I2C initialization instead of mcp2221_open_simple_scan().
     */
    (void)options.usbReadTimeoutMs;
    (void)options.commandRetries;
    (void)options.debugMessages;
    (void)options.tracePackets;
}

Device::~Device() noexcept = default;

void Device::close() noexcept
{
    state_.reset();
}

bool Device::isOpen() const noexcept
{
    return static_cast<bool>(state_);
}

I2cDevice Device::i2cDevice(std::uint8_t address)
{
    return i2cDevice(address, I2cDeviceOptions{});
}

I2cDevice Device::i2cDevice(
    std::uint8_t address,
    const I2cDeviceOptions& options)
{
    if (!state_) {
        throw std::logic_error("Device is closed");
    }

    auto child = std::make_shared<detail::I2cDeviceState>();
    child->device = state_;

    std::lock_guard<std::mutex> lock(state_->mutex());

    const auto order =
        options.byteOrder == ByteOrder::LittleEndian
            ? MCP2221_I2C_BYTE_ORDER_LITTLE
            : MCP2221_I2C_BYTE_ORDER_BIG;

    detail::checkError(mcp2221_i2c_slave_init(
        &child->slave,
        state_->handle(),
        address,
        options.force ? 1 : 0,
        options.speedHz,
        options.registerBytes,
        order));

    return I2cDevice(std::move(child));
}

SmbusDevice Device::smbusDevice(std::uint8_t address)
{
    if (!state_) {
        throw std::logic_error("Device is closed");
    }

    auto child = std::make_shared<detail::SmbusDeviceState>();
    child->device = state_;
    child->address = address;

    std::lock_guard<std::mutex> lock(state_->mutex());

    detail::checkError(mcp2221_smbus_init(
        &child->bus,
        state_->handle(),
        0,
        0,
        0,
        nullptr,
        0));

    return SmbusDevice(std::move(child));
}

GpioPoller Device::gpioPoller()
{
    if (!state_) {
        throw std::logic_error("Device is closed");
    }

    auto child = std::make_unique<detail::GpioPollerState>();
    child->device = state_;
    mcp2221_gpio_poll_init(&child->poll);

    return GpioPoller(std::move(child));
}

void Device::setI2cSpeed(std::uint32_t)
{
    notImplemented("Device::setI2cSpeed");
}

void Device::i2cWrite(
    std::uint8_t,
    const std::uint8_t*,
    std::size_t,
    I2cTransfer)
{
    notImplemented("Device::i2cWrite");
}

void Device::i2cWrite(
    std::uint8_t address,
    const std::vector<std::uint8_t>& data,
    I2cTransfer transfer)
{
    i2cWrite(address, data.data(), data.size(), transfer);
}

std::vector<std::uint8_t> Device::i2cRead(
    std::uint8_t,
    std::size_t,
    I2cTransfer)
{
    notImplemented("Device::i2cRead");
}

I2cStatus Device::i2cStatus()
{
    notImplemented("Device::i2cStatus");
}

void Device::releaseI2c()
{
    notImplemented("Device::releaseI2c");
}

GpioState Device::readGpio()
{
    notImplemented("Device::readGpio");
}

void Device::writeGpio(const GpioWrite&)
{
    notImplemented("Device::writeGpio");
}

void Device::setPinFunction(Pin, PinFunction)
{
    notImplemented("Device::setPinFunction");
}

void Device::configurePins(const PinConfigurations&)
{
    notImplemented("Device::configurePins");
}

void Device::setVdd(double)
{
    notImplemented("Device::setVdd");
}

double Device::vdd() const
{
    notImplemented("Device::vdd");
}

void Device::configureAdc(VoltageReference)
{
    notImplemented("Device::configureAdc");
}

std::array<std::uint16_t, 3> Device::readAdcRaw()
{
    notImplemented("Device::readAdcRaw");
}

std::array<double, 3> Device::readAdcNormalized()
{
    notImplemented("Device::readAdcNormalized");
}

std::array<double, 3> Device::readAdcVolts()
{
    notImplemented("Device::readAdcVolts");
}

void Device::configureDac(VoltageReference)
{
    notImplemented("Device::configureDac");
}

void Device::configureDac(VoltageReference, std::uint8_t)
{
    notImplemented("Device::configureDac");
}

void Device::writeDacRaw(std::uint8_t)
{
    notImplemented("Device::writeDacRaw");
}

void Device::writeDacNormalized(double)
{
    notImplemented("Device::writeDacNormalized");
}

void Device::writeDacVolts(double)
{
    notImplemented("Device::writeDacVolts");
}

void Device::configureClock(ClockDutyCycle, ClockFrequency)
{
    notImplemented("Device::configureClock");
}

bool Device::interruptFlag()
{
    notImplemented("Device::interruptFlag");
}

void Device::clearInterruptFlag()
{
    notImplemented("Device::clearInterruptFlag");
}

void Device::configureInterrupt(InterruptEdge)
{
    notImplemented("Device::configureInterrupt");
}

void Device::configureSram(const SramConfig&)
{
    notImplemented("Device::configureSram");
}

FlashData Device::readFlash(FlashSection)
{
    notImplemented("Device::readFlash");
}

void Device::writeFlash(FlashSection, const FlashData&)
{
    notImplemented("Device::writeFlash");
}

void Device::sendFlashPassword(const FlashPassword&)
{
    notImplemented("Device::sendFlashPassword");
}

FlashInfo Device::flashInfo()
{
    notImplemented("Device::flashInfo");
}

FlashSettings Device::flashSettings()
{
    notImplemented("Device::flashSettings");
}

void Device::saveConfigurationToFlash()
{
    notImplemented("Device::saveConfigurationToFlash");
}

void Device::stageUsbRemoteWakeup(bool)
{
    notImplemented("Device::stageUsbRemoteWakeup");
}

bool Device::usbRemoteWakeup()
{
    notImplemented("Device::usbRemoteWakeup");
}

void Device::stageUsbSelfPowered(bool)
{
    notImplemented("Device::stageUsbSelfPowered");
}

bool Device::usbSelfPowered()
{
    notImplemented("Device::usbSelfPowered");
}

void Device::stageUsbRequestedCurrent(unsigned)
{
    notImplemented("Device::stageUsbRequestedCurrent");
}

unsigned Device::usbRequestedCurrent()
{
    notImplemented("Device::usbRequestedCurrent");
}

std::array<std::uint8_t, 64> Device::rawCommand(
    const std::uint8_t*,
    std::size_t)
{
    notImplemented("Device::rawCommand");
}

} // namespace libeasymcp2221
