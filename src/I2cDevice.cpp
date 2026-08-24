/**
 * @file I2cDevice.cpp
 * @brief I2cDevice implementation and initial stubs.
 */

#include "libeasymcp2221/I2cDevice.h"

#include <stdexcept>
#include <string>
#include <utility>

#include "detail/DeviceState.h"

namespace libeasymcp2221 {

namespace {

[[noreturn]] void notImplemented(const char* operation)
{
    throw std::logic_error(std::string(operation) + " is not implemented yet");
}

} // namespace

I2cDevice::I2cDevice(std::shared_ptr<detail::I2cDeviceState> state)
    : state_(std::move(state))
{
}

std::uint8_t I2cDevice::address() const noexcept
{
    return state_ ? state_->slave.addr : 0;
}

bool I2cDevice::isPresent()
{
    notImplemented("I2cDevice::isPresent");
}

std::vector<std::uint8_t> I2cDevice::read(std::size_t)
{
    notImplemented("I2cDevice::read");
}

void I2cDevice::write(const std::uint8_t*, std::size_t)
{
    notImplemented("I2cDevice::write");
}

void I2cDevice::write(const std::vector<std::uint8_t>& data)
{
    write(data.data(), data.size());
}

std::vector<std::uint8_t> I2cDevice::readRegister(
    std::uint32_t,
    std::size_t)
{
    notImplemented("I2cDevice::readRegister");
}

std::vector<std::uint8_t> I2cDevice::readRegister(
    std::uint32_t,
    std::size_t,
    int,
    ByteOrder)
{
    notImplemented("I2cDevice::readRegister(explicit layout)");
}

void I2cDevice::writeRegister(
    std::uint32_t,
    const std::uint8_t*,
    std::size_t)
{
    notImplemented("I2cDevice::writeRegister");
}

void I2cDevice::writeRegister(
    std::uint32_t reg,
    const std::vector<std::uint8_t>& data)
{
    writeRegister(reg, data.data(), data.size());
}

} // namespace libeasymcp2221
