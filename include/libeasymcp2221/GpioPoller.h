/**
 * @file GpioPoller.h
 * @brief Stateful GPIO change and edge-event polling adapter.
 */

#ifndef LIBEASYMCP2221_CPP_GPIO_POLLER_H
#define LIBEASYMCP2221_CPP_GPIO_POLLER_H

#include <array>
#include <cstddef>
#include <memory>
#include <vector>

#include "Types.h"

namespace libeasymcp2221 {

namespace detail {
class GpioPollerState;
}

/**
 * @brief Stateful GPIO poller.
 *
 * GpioPoller is move-only because copying a polling history would create two
 * independent timelines with initially identical previous-state snapshots.
 */
class GpioPoller {
public:
    GpioPoller(const GpioPoller&) = delete;
    GpioPoller& operator=(const GpioPoller&) = delete;

    GpioPoller(GpioPoller&&) noexcept = default;
    GpioPoller& operator=(GpioPoller&&) noexcept = default;

    ~GpioPoller();

    /**
     * @brief Poll GP0 through GP3 and report state changes.
     *
     * The first successful poll establishes the initial snapshot and reports
     * changed == false for every pin. A disengaged oldValue or newValue means
     * that the pin was not configured as GPIO at that sample.
     */
    std::array<GpioChange, 4> poll();

    /**
     * @brief Poll and return filtered edge events.
     *
     * The first successful poll establishes the initial snapshot and therefore
     * returns no events. Excess matching events beyond @p maxEvents are
     * discarded by the underlying stateful C poller.
     *
     * @param maxEvents Maximum number of events to return.
     * @note Event timestamps use wall-clock time.
     */
    std::vector<GpioEvent> pollEvents(std::size_t maxEvents = 4);

    /**
     * @brief Replace the persistent raw edge-event filter mask.
     *
     * This low-level mask setter is retained initially because it maps exactly
     * to the C API. A higher-level typed GpioEventFilter can replace or augment
     * it later without changing GpioPoller's ownership model.
     */
    void setFilterMask(std::uint16_t mask);

    /** @brief Clear the event filter so that all edges are accepted. */
    void clearFilter();

private:
    friend class Device;

    explicit GpioPoller(std::unique_ptr<detail::GpioPollerState> state);

    std::unique_ptr<detail::GpioPollerState> state_;
};

} // namespace libeasymcp2221

#endif // LIBEASYMCP2221_CPP_GPIO_POLLER_H
