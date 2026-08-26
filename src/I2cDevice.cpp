/**
 * @file I2cDevice.cpp
 * @brief I2cDevice implementation.
 */

#include "libeasymcp2221++/I2cDevice.h"

#include <stdexcept>
#include <utility>

#include "detail/CheckError.h"
#include "detail/DeviceState.h"

namespace libeasymcp2221 {

namespace {

void requireState(const std::shared_ptr<detail::I2cDeviceState>& state) {
	if (!state || !state->device) {
		throw std::logic_error("I2cDevice has no device state");
	}
}

int toNativeRegisterWidth(RegisterWidth width) {
	switch (width) {
		case RegisterWidth::Bits8:
			return 1;
		case RegisterWidth::Bits16:
			return 2;
		case RegisterWidth::Bits24:
			return 3;
		case RegisterWidth::Bits32:
			return 4;
	}

	detail::throwInvalid("Unknown I2C register width");
}

mcp2221_i2c_byte_order_t toNativeByteOrder(ByteOrder byteOrder) {
	switch (byteOrder) {
		case ByteOrder::BigEndian:
			return MCP2221_I2C_BYTE_ORDER_BIG;
		case ByteOrder::LittleEndian:
			return MCP2221_I2C_BYTE_ORDER_LITTLE;
	}

	detail::throwInvalid("Unknown I2C register byte order");
}

}  // namespace

I2cDevice::I2cDevice(std::shared_ptr<detail::I2cDeviceState> state) : state_(std::move(state)) {}

std::uint8_t I2cDevice::address() const noexcept {
	return state_ ? state_->slave.addr : 0;
}

bool I2cDevice::isPresent() {
	requireState(state_);

	int present = 0;
	std::lock_guard<std::mutex> lock(state_->device->mutex());
	detail::checkError(mcp2221_i2c_slave_check_present(&state_->slave, &present), "Checking I2C target presence");

	return present != 0;
}

std::vector<std::uint8_t> I2cDevice::read(std::size_t size) {
	requireState(state_);

	if (size == 0 || size > 256) {
		detail::throwInvalid("I2cDevice read size must be from 1 through 256 bytes");
	}

	std::vector<std::uint8_t> data(size);

	std::lock_guard<std::mutex> lock(state_->device->mutex());
	detail::checkError(mcp2221_i2c_slave_read(&state_->slave, data.data(), data.size()), "Reading from I2C target");

	return data;
}

void I2cDevice::write(const std::uint8_t* data, std::size_t size) {
	requireState(state_);

	if (data == nullptr) {
		detail::throwInvalid("I2cDevice write pointer must not be null");
	}
	if (size == 0 || size > 256) {
		detail::throwInvalid("I2cDevice write size must be from 1 through 256 bytes");
	}

	std::lock_guard<std::mutex> lock(state_->device->mutex());
	detail::checkError(mcp2221_i2c_slave_write(&state_->slave, data, size), "Writing to I2C target");
}

void I2cDevice::write(const std::vector<std::uint8_t>& data) {
	write(data.data(), data.size());
}

std::vector<std::uint8_t> I2cDevice::readRegister(std::uint32_t reg, std::size_t size) {
	requireState(state_);

	if (size == 0 || size > 256) {
		detail::throwInvalid("I2cDevice register read size must be from 1 through 256 bytes");
	}

	std::vector<std::uint8_t> data(size);

	std::lock_guard<std::mutex> lock(state_->device->mutex());
	detail::checkError(mcp2221_i2c_slave_read_register(&state_->slave, reg, data.data(), data.size(), 0,
													   MCP2221_I2C_BYTE_ORDER_DEFAULT),
					   "Reading I2C target register");

	return data;
}

std::vector<std::uint8_t> I2cDevice::readRegister(std::uint32_t reg, std::size_t size, RegisterWidth registerWidth,
												  ByteOrder byteOrder) {
	requireState(state_);

	if (size == 0 || size > 256) {
		detail::throwInvalid("I2cDevice register read size must be from 1 through 256 bytes");
	}
	std::vector<std::uint8_t> data(size);

	std::lock_guard<std::mutex> lock(state_->device->mutex());
	detail::checkError(
		mcp2221_i2c_slave_read_register(&state_->slave, reg, data.data(), data.size(),
										toNativeRegisterWidth(registerWidth), toNativeByteOrder(byteOrder)),
		"Reading I2C target register");

	return data;
}

void I2cDevice::writeRegister(std::uint32_t reg, const std::uint8_t* data, std::size_t size) {
	requireState(state_);

	if (data == nullptr && size != 0) {
		detail::throwInvalid("I2cDevice register write pointer must not be null for nonzero size");
	}
	if (size > 256) {
		detail::throwInvalid("I2cDevice register write size must not exceed 256 bytes");
	}

	std::lock_guard<std::mutex> lock(state_->device->mutex());
	detail::checkError(
		mcp2221_i2c_slave_write_register(&state_->slave, reg, data, size, 0, MCP2221_I2C_BYTE_ORDER_DEFAULT),
		"Writing I2C target register");
}

void I2cDevice::writeRegister(std::uint32_t reg, const std::uint8_t* data, std::size_t size,
							  RegisterWidth registerWidth, ByteOrder byteOrder) {
	requireState(state_);

	if (data == nullptr && size != 0) {
		detail::throwInvalid("I2cDevice register write pointer must not be null for nonzero size");
	}
	if (size > 256) {
		detail::throwInvalid("I2cDevice register write size must not exceed 256 bytes");
	}
	std::lock_guard<std::mutex> lock(state_->device->mutex());
	detail::checkError(
		mcp2221_i2c_slave_write_register(&state_->slave, reg, data, size, toNativeRegisterWidth(registerWidth),
										 toNativeByteOrder(byteOrder)),
		"Writing I2C target register");
}

void I2cDevice::writeRegister(std::uint32_t reg, const std::vector<std::uint8_t>& data) {
	writeRegister(reg, data.data(), data.size());
}

}  // namespace libeasymcp2221
