/**
 * @file I2cDevice.h
 * @brief Stateful adapter for one I2C target device.
 */

#ifndef LIBEASYMCP2221_CPP_I2C_DEVICE_H
#define LIBEASYMCP2221_CPP_I2C_DEVICE_H

#include "Export.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "Types.h"

namespace libeasymcp2221 {

namespace detail {
class I2cDeviceState;
}

/** @brief Options used when creating an I2C target adapter. */
struct I2cDeviceOptions {
	bool force = false;
	std::uint32_t speedHz = 100000;
	RegisterWidth registerWidth = RegisterWidth::Bits8;
	ByteOrder byteOrder = ByteOrder::BigEndian;
};

/**
 * @brief Stateful adapter for one 7-bit I2C target.
 *
 * Instances are copyable. Copies refer to the same underlying MCP2221 device
 * and share its lifetime and synchronization state.
 */
class LIBEASYMCP2221_CPP_API I2cDevice {
  public:
	I2cDevice(const I2cDevice&) = default;
	I2cDevice& operator=(const I2cDevice&) = default;
	I2cDevice(I2cDevice&&) noexcept = default;
	I2cDevice& operator=(I2cDevice&&) noexcept = default;
	~I2cDevice() = default;

	/**
	 * @brief Return the configured 7-bit I2C address.
	 * @return Configured 7-bit target address.
	 */
	[[nodiscard]] std::uint8_t address() const noexcept;

	/**
	 * @brief Check whether the target acknowledges its address.
	 * @return true if present, false for an address NACK.
	 * @throws Error for transport or protocol failures.
	 */
	[[nodiscard]] bool isPresent();

	/**
	 * @brief Read bytes directly from the target.
	 * @param size Number of bytes to read.
	 * @return Received payload bytes.
	 */
	[[nodiscard]] std::vector<std::uint8_t> read(std::size_t size);

	/**
	 * @brief Write bytes directly to the target.
	 * @param data Payload pointer.
	 * @param size Number of payload bytes.
	 */
	void write(const std::uint8_t* data, std::size_t size);

	/**
	 * @brief Convenience overload for std::vector payloads.
	 * @param data Payload bytes.
	 */
	void write(const std::vector<std::uint8_t>& data);

	/**
	 * @brief Read bytes beginning at a register using the default layout.
	 * @param reg Register address.
	 * @param size Number of bytes to read.
	 * @return Received register payload bytes.
	 */
	[[nodiscard]] std::vector<std::uint8_t> readRegister(std::uint32_t reg, std::size_t size);

	/**
	 * @brief Read bytes beginning at a register with an explicit layout.
	 * @param reg Register address.
	 * @param size Number of bytes to read.
	 * @param registerWidth Register-address width.
	 * @param byteOrder Register-address byte order.
	 * @return Received register payload bytes.
	 */
	[[nodiscard]] std::vector<std::uint8_t> readRegister(std::uint32_t reg, std::size_t size,
														 RegisterWidth registerWidth, ByteOrder byteOrder);

	/**
	 * @brief Write bytes beginning at a register using the default layout.
	 * @param reg Register address.
	 * @param data Payload pointer.
	 * @param size Number of payload bytes.
	 */
	void writeRegister(std::uint32_t reg, const std::uint8_t* data, std::size_t size);

	/**
	 * @brief Write bytes beginning at a register with an explicit layout.
	 * @param reg Register address.
	 * @param data Payload pointer, or nullptr when size is zero.
	 * @param size Number of payload bytes.
	 * @param registerWidth Register-address width.
	 * @param byteOrder Register-address byte order.
	 * @throws Error on validation or transfer failure.
	 */
	void writeRegister(std::uint32_t reg, const std::uint8_t* data, std::size_t size, RegisterWidth registerWidth,
					   ByteOrder byteOrder);

	/**
	 * @brief Convenience overload for std::vector payloads.
	 * @param reg Register address.
	 * @param data Payload bytes.
	 */
	void writeRegister(std::uint32_t reg, const std::vector<std::uint8_t>& data);

  private:
	friend class Device;

	/**
	 * @brief Construct an adapter from shared internal target state.
	 * @param state Shared internal I2C target state.
	 */
	explicit I2cDevice(std::shared_ptr<detail::I2cDeviceState> state);

	std::shared_ptr<detail::I2cDeviceState> state_;
};

}  // namespace libeasymcp2221

#endif	// LIBEASYMCP2221_CPP_I2C_DEVICE_H
