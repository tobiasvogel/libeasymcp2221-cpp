/**
 * @file DeviceState.h
 * @brief Internal shared MCP2221 handle and synchronization state.
 */

#ifndef LIBEASYMCP2221_CPP_DETAIL_DEVICE_STATE_H
#define LIBEASYMCP2221_CPP_DETAIL_DEVICE_STATE_H

#include <cstdint>
#include <memory>
#include <mutex>

extern "C" {
#include <libeasymcp2221/mcp2221.h>
#include <libeasymcp2221/mcp2221_analog.h>
#include <libeasymcp2221/mcp2221_constants.h>
#include <libeasymcp2221/mcp2221_flash.h>
#include <libeasymcp2221/mcp2221_flash_info.h>
#include <libeasymcp2221/mcp2221_flash_settings.h>
#include <libeasymcp2221/mcp2221_gpio.h>
#include <libeasymcp2221/mcp2221_gpio_poll.h>
#include <libeasymcp2221/mcp2221_i2c_slave.h>
#include <libeasymcp2221/mcp2221_pin.h>
#include <libeasymcp2221/mcp2221_smbus.h>
#include <libeasymcp2221/mcp2221_sram.h>
#include <libeasymcp2221/mcp2221_usb.h>
}

namespace libeasymcp2221 {
namespace detail {

/**
 * @brief Shared owner of one C mcp2221_t reference.
 *
 * All high-level C++ objects that access the same C handle share this state and
 * therefore the same operation mutex.
 */
class DeviceState {
  public:
	/**
	 * @brief Construct shared state for an already-open C device handle.
	 * @param handle Owned C device handle. May be nullptr only for an empty
	 *               internal state.
	 */
	explicit DeviceState(mcp2221_t* handle) noexcept;
	~DeviceState() noexcept;

	DeviceState(const DeviceState&) = delete;
	DeviceState& operator=(const DeviceState&) = delete;

	/**
	 * @brief Return the underlying C device handle.
	 * @return Borrowed pointer to the owned C handle.
	 */
	mcp2221_t* handle() noexcept;

	/**
	 * @brief Return the mutex serializing operations on this C handle.
	 * @return Reference to the shared operation mutex.
	 */
	std::mutex& mutex() noexcept;

  private:
	mcp2221_t* handle_ = nullptr;
	std::mutex mutex_;
};

/** @brief Internal state backing a copyable I2cDevice adapter. */
class I2cDeviceState {
  public:
	std::shared_ptr<DeviceState> device;
	mcp2221_i2c_slave_t slave{};
};

/** @brief Internal state backing a copyable SmbusDevice adapter. */
class SmbusDeviceState {
  public:
	~SmbusDeviceState() noexcept;

	std::shared_ptr<DeviceState> device;
	mcp2221_smbus_t bus{};
	std::uint8_t address = 0;
};

/** @brief Internal state backing a move-only GpioPoller. */
class GpioPollerState {
  public:
	std::shared_ptr<DeviceState> device;
	mcp2221_gpio_poll_state_t poll{};
};

}  // namespace detail
}  // namespace libeasymcp2221

#endif	// LIBEASYMCP2221_CPP_DETAIL_DEVICE_STATE_H
