/**
 * @file GpioPoller.cpp
 * @brief GpioPoller implementation and initial stubs.
 */

#include "libeasymcp2221/GpioPoller.h"

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

GpioPoller::GpioPoller(std::unique_ptr<detail::GpioPollerState> state)
    : state_(std::move(state))
{
}

GpioPoller::~GpioPoller() = default;

std::array<GpioChange, 4> GpioPoller::poll()
{
    notImplemented("GpioPoller::poll");
}

std::vector<GpioEvent> GpioPoller::pollEvents(std::size_t)
{
    notImplemented("GpioPoller::pollEvents");
}

void GpioPoller::setFilterMask(std::uint16_t mask)
{
    if (!state_) {
        throw std::logic_error("GpioPoller has no state");
    }

    mcp2221_gpio_poll_set_filter_mask(&state_->poll, mask);
}

void GpioPoller::clearFilter()
{
    setFilterMask(0);
}

std::string GpioEvent::id() const
{
    const auto pinNumber = static_cast<unsigned>(pin);
    const char* suffix = edge == GpioEdge::Rising ? "_RISE" : "_FALL";
    return "GPIO" + std::to_string(pinNumber) + suffix;
}

} // namespace libeasymcp2221
