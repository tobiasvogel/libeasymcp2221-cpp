/**
 * @file GpioPoller.cpp
 * @brief GpioPoller implementation.
 */

#include "libeasymcp2221++/GpioPoller.h"

#include <array>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "detail/CheckError.h"
#include "detail/DeviceState.h"

namespace libeasymcp2221 {

namespace {

void requireState(const std::unique_ptr<detail::GpioPollerState>& state) {
	if (!state || !state->device) {
		throw std::logic_error("GpioPoller has no device state");
	}
}

std::optional<bool> toOptionalGpioValue(int value, const char* operation) {
	if (value == -1) {
		return std::nullopt;
	}
	if (value == 0) {
		return false;
	}
	if (value == 1) {
		return true;
	}

	throw Error(ErrorCode::Protocol, static_cast<int>(MCP2221_ERR_PROTOCOL),
				std::string(operation) + ": invalid GPIO value returned by C library");
}

Pin toPin(std::uint8_t gpio) {
	switch (gpio) {
		case 0:
			return Pin::GP0;
		case 1:
			return Pin::GP1;
		case 2:
			return Pin::GP2;
		case 3:
			return Pin::GP3;
	}

	throw Error(ErrorCode::Protocol, static_cast<int>(MCP2221_ERR_PROTOCOL),
				"Polling GPIO events: invalid GPIO index returned by C library");
}

GpioEdge toEdge(mcp2221_gpio_event_type_t type) {
	switch (type) {
		case MCP2221_GPIO_EVENT_RISE:
			return GpioEdge::Rising;
		case MCP2221_GPIO_EVENT_FALL:
			return GpioEdge::Falling;
	}

	throw Error(ErrorCode::Protocol, static_cast<int>(MCP2221_ERR_PROTOCOL),
				"Polling GPIO events: invalid edge type returned by C library");
}

}  // namespace

std::string GpioEvent::id() const
{
	const char* pinName = nullptr;
	switch (pin) {
		case Pin::GP0:
			pinName = "GPIO0";
			break;
		case Pin::GP1:
			pinName = "GPIO1";
			break;
		case Pin::GP2:
			pinName = "GPIO2";
			break;
		case Pin::GP3:
			pinName = "GPIO3";
			break;
	}

	if (pinName == nullptr) {
		detail::throwInvalid("Unknown GPIO event pin");
	}

	const char* edgeName = nullptr;
	switch (edge) {
		case GpioEdge::Rising:
			edgeName = "RISE";
			break;
		case GpioEdge::Falling:
			edgeName = "FALL";
			break;
		default:
			detail::throwInvalid("Unknown GPIO event edge");
	}

	return std::string(pinName) + "_" + edgeName;
}

GpioPoller::GpioPoller(std::unique_ptr<detail::GpioPollerState> state) : state_(std::move(state)) {}

GpioPoller::GpioPoller(GpioPoller&&) noexcept = default;

GpioPoller& GpioPoller::operator=(GpioPoller&&) noexcept = default;

GpioPoller::~GpioPoller() noexcept = default;

void GpioPoller::setFilter(const GpioEventFilter& filter) {
	requireState(state_);

	std::uint16_t mask = 0;
	for (std::size_t pin = 0; pin < 4; ++pin) {
		if (filter.rising[pin]) {
			mask |= static_cast<std::uint16_t>(MCP2221_GPIO_POLL_MASK_RISE(pin));
		}
		if (filter.falling[pin]) {
			mask |= static_cast<std::uint16_t>(MCP2221_GPIO_POLL_MASK_FALL(pin));
		}
	}

	std::lock_guard<std::mutex> lock(state_->device->mutex());
	mcp2221_gpio_poll_set_filter_mask(&state_->poll, mask);
}

void GpioPoller::clearFilter() {
	setFilter(GpioEventFilter{});
}

std::array<GpioChange, 4> GpioPoller::poll() {
	requireState(state_);

	std::array<mcp2221_gpio_change_t, 4> native{};
	{
		std::lock_guard<std::mutex> lock(state_->device->mutex());
		detail::checkError(mcp2221_gpio_poll(state_->device->handle(), &state_->poll, native.data()),
						   "Polling GPIO changes");
	}

	std::array<GpioChange, 4> result{};
	for (std::size_t i = 0; i < result.size(); ++i) {
		result[i].oldValue = toOptionalGpioValue(native[i].old_value, "Polling GPIO changes");
		result[i].newValue = toOptionalGpioValue(native[i].new_value, "Polling GPIO changes");
		result[i].changed = native[i].changed != 0;
	}

	return result;
}

std::vector<GpioEvent> GpioPoller::pollEvents(std::size_t maxEvents) {
	requireState(state_);

	constexpr std::size_t MaxEventsPerPoll = 4;
	const auto capacity = maxEvents < MaxEventsPerPoll ? maxEvents : MaxEventsPerPoll;

	std::array<mcp2221_gpio_event_t, MaxEventsPerPoll> native{};
	auto* buffer = capacity == 0 ? nullptr : native.data();
	std::vector<GpioEvent> result(capacity);

	int count = 0;
	{
		std::lock_guard<std::mutex> lock(state_->device->mutex());
		count = mcp2221_gpio_poll_events(state_->device->handle(), &state_->poll, nullptr, buffer, capacity);
	}

	if (count < 0) {
		detail::checkError(static_cast<mcp2221_error_code_t>(count), "Polling GPIO events");
	}

	const auto eventCount = static_cast<std::size_t>(count);
	if (eventCount > capacity) {
		throw Error(ErrorCode::Protocol, static_cast<int>(MCP2221_ERR_PROTOCOL),
					"Polling GPIO events: C library returned too many events");
	}

	for (std::size_t i = 0; i < eventCount; ++i) {
		result[i].pin = toPin(native[i].gpio);
		result[i].edge = toEdge(native[i].type);
		result[i].time = native[i].time;
		result[i].previousTime = native[i].last_time;
	}

	result.resize(eventCount);
	return result;
}

}  // namespace libeasymcp2221
