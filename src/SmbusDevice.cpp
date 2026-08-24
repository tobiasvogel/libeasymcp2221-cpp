/**
 * @file SmbusDevice.cpp
 * @brief SmbusDevice implementation and initial stubs.
 */

#include "libeasymcp2221/SmbusDevice.h"

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

SmbusDevice::SmbusDevice(std::shared_ptr<detail::SmbusDeviceState> state)
    : state_(std::move(state))
{
}

std::uint8_t SmbusDevice::address() const noexcept
{
    return state_ ? state_->address : 0;
}

std::uint8_t SmbusDevice::readByte()
{
    notImplemented("SmbusDevice::readByte");
}

void SmbusDevice::writeByte(std::uint8_t)
{
    notImplemented("SmbusDevice::writeByte");
}

std::uint8_t SmbusDevice::readByteData(std::uint8_t)
{
    notImplemented("SmbusDevice::readByteData");
}

void SmbusDevice::writeByteData(std::uint8_t, std::uint8_t)
{
    notImplemented("SmbusDevice::writeByteData");
}

std::int16_t SmbusDevice::readWordData(std::uint8_t)
{
    notImplemented("SmbusDevice::readWordData");
}

void SmbusDevice::writeWordData(std::uint8_t, std::int16_t)
{
    notImplemented("SmbusDevice::writeWordData");
}

std::int16_t SmbusDevice::processCall(std::uint8_t, std::int16_t)
{
    notImplemented("SmbusDevice::processCall");
}

std::vector<std::uint8_t> SmbusDevice::readBlockData(std::uint8_t)
{
    notImplemented("SmbusDevice::readBlockData");
}

void SmbusDevice::writeBlockData(
    std::uint8_t,
    const std::vector<std::uint8_t>&)
{
    notImplemented("SmbusDevice::writeBlockData");
}

std::vector<std::uint8_t> SmbusDevice::blockProcessCall(
    std::uint8_t,
    const std::vector<std::uint8_t>&)
{
    notImplemented("SmbusDevice::blockProcessCall");
}

std::vector<std::uint8_t> SmbusDevice::readI2cBlockData(
    std::uint8_t,
    std::size_t)
{
    notImplemented("SmbusDevice::readI2cBlockData");
}

void SmbusDevice::writeI2cBlockData(
    std::uint8_t,
    const std::vector<std::uint8_t>&)
{
    notImplemented("SmbusDevice::writeI2cBlockData");
}

} // namespace libeasymcp2221
