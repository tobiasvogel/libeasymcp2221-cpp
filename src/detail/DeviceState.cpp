/**
 * @file DeviceState.cpp
 * @brief Internal DeviceState implementation.
 */

#include "detail/DeviceState.h"

namespace libeasymcp2221 {
namespace detail {

DeviceState::DeviceState(mcp2221_t* handle) noexcept : handle_(handle) {}

DeviceState::~DeviceState() noexcept {
	if (handle_ != nullptr) {
		mcp2221_close(handle_);
	}
}

mcp2221_t* DeviceState::handle() noexcept {
	return handle_;
}

std::mutex& DeviceState::mutex() noexcept {
	return mutex_;
}

SmbusDeviceState::~SmbusDeviceState() noexcept {
	mcp2221_smbus_close(&bus);
}

}  // namespace detail
}  // namespace libeasymcp2221
