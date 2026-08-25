/**
 * @file SmbusDevice.cpp
 * @brief SmbusDevice implementation.
 */

#include "libeasymcp2221++/SmbusDevice.h"

#include <array>
#include <stdexcept>
#include <utility>

#include "detail/CheckError.h"
#include "detail/DeviceState.h"

namespace libeasymcp2221 {

namespace {

constexpr std::size_t MaxBlockSize = MCP2221_I2C_SMBUS_BLOCK_MAX;

void requireState(const std::shared_ptr<detail::SmbusDeviceState>& state)
{
    if (!state || !state->device) {
        throw std::logic_error("SmbusDevice has no device state");
    }
}

void validateWriteBuffer(const std::vector<std::uint8_t>& data)
{
    if (data.size() > MaxBlockSize) {
        detail::throwInvalid("SMBus block size must not exceed 255 bytes");
    }
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
    requireState(state_);

    std::uint8_t value = 0;
    std::lock_guard<std::mutex> lock(state_->device->mutex());
    detail::checkError(
        mcp2221_smbus_read_byte(
            &state_->bus,
            state_->address,
            &value),
        "Reading SMBus byte");

    return value;
}

void SmbusDevice::writeByte(std::uint8_t value)
{
    requireState(state_);

    std::lock_guard<std::mutex> lock(state_->device->mutex());
    detail::checkError(
        mcp2221_smbus_write_byte(
            &state_->bus,
            state_->address,
            value),
        "Writing SMBus byte");
}

std::uint8_t SmbusDevice::readByteData(std::uint8_t reg)
{
    requireState(state_);

    std::uint8_t value = 0;
    std::lock_guard<std::mutex> lock(state_->device->mutex());
    detail::checkError(
        mcp2221_smbus_read_byte_data(
            &state_->bus,
            state_->address,
            reg,
            &value),
        "Reading SMBus byte data");

    return value;
}

void SmbusDevice::writeByteData(std::uint8_t reg, std::uint8_t value)
{
    requireState(state_);

    std::lock_guard<std::mutex> lock(state_->device->mutex());
    detail::checkError(
        mcp2221_smbus_write_byte_data(
            &state_->bus,
            state_->address,
            reg,
            value),
        "Writing SMBus byte data");
}

std::int16_t SmbusDevice::readWordData(std::uint8_t reg)
{
    requireState(state_);

    std::int16_t value = 0;
    std::lock_guard<std::mutex> lock(state_->device->mutex());
    detail::checkError(
        mcp2221_smbus_read_word_data(
            &state_->bus,
            state_->address,
            reg,
            &value),
        "Reading SMBus word data");

    return value;
}

void SmbusDevice::writeWordData(std::uint8_t reg, std::int16_t value)
{
    requireState(state_);

    std::lock_guard<std::mutex> lock(state_->device->mutex());
    detail::checkError(
        mcp2221_smbus_write_word_data(
            &state_->bus,
            state_->address,
            reg,
            value),
        "Writing SMBus word data");
}

std::int16_t SmbusDevice::processCall(std::uint8_t reg, std::int16_t value)
{
    requireState(state_);

    std::int16_t response = 0;
    std::lock_guard<std::mutex> lock(state_->device->mutex());
    detail::checkError(
        mcp2221_smbus_process_call(
            &state_->bus,
            state_->address,
            reg,
            value,
            &response),
        "Performing SMBus process call");

    return response;
}

std::vector<std::uint8_t> SmbusDevice::readBlockData(std::uint8_t reg)
{
    requireState(state_);

    std::array<std::uint8_t, MaxBlockSize> buffer{};
    std::size_t length = 0;

    std::lock_guard<std::mutex> lock(state_->device->mutex());
    detail::checkError(
        mcp2221_smbus_read_block_data(
            &state_->bus,
            state_->address,
            reg,
            buffer.data(),
            &length),
        "Reading SMBus block data");

    if (length > buffer.size()) {
        detail::throwInvalid("SMBus target returned an invalid block length");
    }

    return std::vector<std::uint8_t>(
        buffer.begin(),
        buffer.begin() + static_cast<std::ptrdiff_t>(length));
}

void SmbusDevice::writeBlockData(
    std::uint8_t reg,
    const std::vector<std::uint8_t>& data)
{
    requireState(state_);
    validateWriteBuffer(data);

    const std::uint8_t emptyPayload = 0;
    const std::uint8_t* ptr =
        data.empty() ? &emptyPayload : data.data();

    std::lock_guard<std::mutex> lock(state_->device->mutex());
    detail::checkError(
        mcp2221_smbus_write_block_data(
            &state_->bus,
            state_->address,
            reg,
            ptr,
            data.size()),
        "Writing SMBus block data");
}

std::vector<std::uint8_t> SmbusDevice::blockProcessCall(
    std::uint8_t reg,
    const std::vector<std::uint8_t>& data)
{
    requireState(state_);
    validateWriteBuffer(data);

    std::array<std::uint8_t, MaxBlockSize> response{};
    std::size_t responseLength = 0;
    const std::uint8_t emptyPayload = 0;
    const std::uint8_t* ptr =
        data.empty() ? &emptyPayload : data.data();

    std::lock_guard<std::mutex> lock(state_->device->mutex());
    detail::checkError(
        mcp2221_smbus_block_process_call(
            &state_->bus,
            state_->address,
            reg,
            ptr,
            data.size(),
            response.data(),
            &responseLength),
        "Performing SMBus block process call");

    if (responseLength > response.size()) {
        detail::throwInvalid("SMBus target returned an invalid block length");
    }

    return std::vector<std::uint8_t>(
        response.begin(),
        response.begin() + static_cast<std::ptrdiff_t>(responseLength));
}

std::vector<std::uint8_t> SmbusDevice::readI2cBlockData(
    std::uint8_t reg,
    std::size_t size)
{
    requireState(state_);

    if (size == 0 || size > MaxBlockSize) {
        detail::throwInvalid(
            "SMBus I2C block read size must be from 1 through 255 bytes");
    }

    std::vector<std::uint8_t> data(size);

    std::lock_guard<std::mutex> lock(state_->device->mutex());
    detail::checkError(
        mcp2221_smbus_read_i2c_block_data(
            &state_->bus,
            state_->address,
            reg,
            data.data(),
            data.size()),
        "Reading SMBus I2C block data");

    return data;
}

void SmbusDevice::writeI2cBlockData(
    std::uint8_t reg,
    const std::vector<std::uint8_t>& data)
{
    requireState(state_);
    validateWriteBuffer(data);

    const std::uint8_t emptyPayload = 0;
    const std::uint8_t* ptr =
        data.empty() ? &emptyPayload : data.data();

    std::lock_guard<std::mutex> lock(state_->device->mutex());
    detail::checkError(
        mcp2221_smbus_write_i2c_block_data(
            &state_->bus,
            state_->address,
            reg,
            ptr,
            data.size()),
        "Writing SMBus I2C block data");
}

} // namespace libeasymcp2221
