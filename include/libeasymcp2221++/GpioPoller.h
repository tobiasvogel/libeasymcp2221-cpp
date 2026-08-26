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

	~GpioPoller() noexcept;

	/**
	 * @brief Poll GP0 through GP3 and report state changes.
	 *
	 * The first successful poll establishes the initial snapshot and reports
	 * changed == false for every pin. A disengaged oldValue or newValue means
	 * that the pin was not configured as GPIO at that sample.
	 *
	 * @return Change information for GP0 through GP3.
	 */
	[[nodiscard]] std::array<GpioChange, 4> poll();

	/**
	 * @brief Poll and return filtered edge events.
	 *
	 * The first successful poll establishes the initial snapshot and therefore
	 * returns no events. Excess matching events beyond @p maxEvents are
	 * discarded by the underlying stateful C poller.
	 *
	 * @param maxEvents Maximum number of events to return.
	 * @return Filtered GPIO edge events observed during this poll.
	 * @note Event timestamps use wall-clock time.
	 */
	[[nodiscard]] std::vector<GpioEvent> pollEvents(std::size_t maxEvents = 4);

	/**
	 * @brief Replace the persistent edge-event selection.
	 *
	 * A default-constructed filter accepts all events. Otherwise only events
	 * whose corresponding rising/falling entry is true are emitted.
	 *
	 * @param filter Typed persistent edge-event selection.
	 */
	void setFilter(const GpioEventFilter& filter);

	/** @brief Clear the event filter so that all edges are accepted. */
	void clearFilter();

  private:
	friend class Device;

	/**
	 * @brief Construct a poller from internal device state.
	 * @param state Owned internal polling state.
	 */
	explicit GpioPoller(std::unique_ptr<detail::GpioPollerState> state);

	std::unique_ptr<detail::GpioPollerState> state_;
};

}  // namespace libeasymcp2221

#endif	// LIBEASYMCP2221_CPP_GPIO_POLLER_H
