/**
 * @file SmbusDevice.h
 * @brief Stateful adapter for one SMBus target device.
 */

#ifndef LIBEASYMCP2221_CPP_SMBUS_DEVICE_H
#define LIBEASYMCP2221_CPP_SMBUS_DEVICE_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace libeasymcp2221 {

namespace detail {
class SmbusDeviceState;
}

/**
 * @brief Stateful adapter for one SMBus target.
 *
 * Instances are copyable. Copies share the underlying MCP2221 device and
 * adapter state.
 */
class SmbusDevice {
public:
    SmbusDevice(const SmbusDevice&) = default;
    SmbusDevice& operator=(const SmbusDevice&) = default;
    SmbusDevice(SmbusDevice&&) noexcept = default;
    SmbusDevice& operator=(SmbusDevice&&) noexcept = default;
    ~SmbusDevice() = default;

    /** @brief Return the configured 7-bit target address. */
    std::uint8_t address() const noexcept;

    /**
     * @brief Read one byte directly from the target.
     * @return Byte read from the target.
     * @throws Error on transfer failure.
     */
    std::uint8_t readByte();

    /**
     * @brief Write one byte directly to the target.
     * @param value Byte to write.
     * @throws Error on transfer failure.
     */
    void writeByte(std::uint8_t value);

    /**
     * @brief Read one byte from an SMBus command/register.
     * @param reg SMBus command/register byte.
     * @return Byte read from the target.
     * @throws Error on transfer failure.
     */
    std::uint8_t readByteData(std::uint8_t reg);

    /**
     * @brief Write one byte to an SMBus command/register.
     * @param reg SMBus command/register byte.
     * @param value Byte to write.
     * @throws Error on transfer failure.
     */
    void writeByteData(std::uint8_t reg, std::uint8_t value);

    /**
     * @brief Read one signed 16-bit SMBus word.
     * @param reg SMBus command/register byte.
     * @return Decoded signed word value.
     * @throws Error on transfer failure.
     */
    std::int16_t readWordData(std::uint8_t reg);

    /**
     * @brief Write one signed 16-bit SMBus word.
     * @param reg SMBus command/register byte.
     * @param value Word value to write.
     * @throws Error on transfer failure.
     */
    void writeWordData(std::uint8_t reg, std::int16_t value);

    /**
     * @brief Perform an SMBus process call.
     * @param reg SMBus command/register byte.
     * @param value Word value to send.
     * @return Word response from the target.
     * @throws Error on transfer failure.
     */
    std::int16_t processCall(std::uint8_t reg, std::int16_t value);

    /**
     * @brief Read a length-prefixed SMBus block.
     * @param reg SMBus command/register byte.
     * @return Payload bytes without the length prefix.
     * @throws Error on transfer failure.
     */
    std::vector<std::uint8_t> readBlockData(std::uint8_t reg);

    /**
     * @brief Write a length-prefixed SMBus block.
     * @param reg SMBus command/register byte.
     * @param data Payload bytes.
     * @throws Error on transfer failure or invalid block size.
     */
    void writeBlockData(std::uint8_t reg, const std::vector<std::uint8_t>& data);

    /**
     * @brief Perform an SMBus block process call.
     * @param reg SMBus command/register byte.
     * @param data Payload bytes to send.
     * @return Response payload.
     * @throws Error on transfer failure.
     */
    std::vector<std::uint8_t> blockProcessCall(
        std::uint8_t reg,
        const std::vector<std::uint8_t>& data);

    /**
     * @brief Read a fixed-length I2C-style block through SMBus helpers.
     * @param reg SMBus command/register byte.
     * @param size Number of bytes to read.
     * @return Requested data bytes.
     * @throws Error on transfer failure or invalid size.
     */
    std::vector<std::uint8_t> readI2cBlockData(
        std::uint8_t reg,
        std::size_t size);

    /**
     * @brief Write a fixed-length I2C-style block through SMBus helpers.
     * @param reg SMBus command/register byte.
     * @param data Payload bytes.
     * @throws Error on transfer failure or invalid size.
     */
    void writeI2cBlockData(
        std::uint8_t reg,
        const std::vector<std::uint8_t>& data);

private:
    friend class Device;

    explicit SmbusDevice(std::shared_ptr<detail::SmbusDeviceState> state);

    std::shared_ptr<detail::SmbusDeviceState> state_;
};

} // namespace libeasymcp2221

#endif // LIBEASYMCP2221_CPP_SMBUS_DEVICE_H
