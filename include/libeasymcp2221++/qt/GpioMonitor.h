/**
 * @file GpioMonitor.h
 * @brief Qt event-loop adapter for GPIO edge monitoring.
 */

#ifndef LIBEASYMCP2221_CPP_QT_GPIO_MONITOR_H
#define LIBEASYMCP2221_CPP_QT_GPIO_MONITOR_H

#include <chrono>
#include <cstddef>

#include <QObject>
#include <QTimer>

#include <libeasymcp2221++/GpioPoller.h>
#include <libeasymcp2221++/Types.h>

#include "ErrorInfo.h"
#include "QtTypes.h"

namespace libeasymcp2221::qt {

/**
 * @brief Periodically polls GPIO edges and exposes them as Qt signals.
 *
 * GpioMonitor owns a GpioPoller and drives it from a QTimer. GPIO edge
 * detection and polling state remain implemented by the libeasymcp2221++
 * core layer.
 *
 * The object follows normal QObject thread-affinity rules. The timer and all
 * polling operations execute in the thread owning this object.
 */
class GpioMonitor : public QObject {
    Q_OBJECT

  public:
    /**
     * @brief Construct a monitor from an existing GPIO poller.
     *
     * Ownership of @p poller is transferred to this object.
     *
     * @param poller Stateful GPIO poller.
     * @param parent Optional QObject parent.
     */
    explicit GpioMonitor(
        GpioPoller poller,
        QObject* parent = nullptr);

    ~GpioMonitor() override;

    GpioMonitor(const GpioMonitor&) = delete;
    GpioMonitor& operator=(const GpioMonitor&) = delete;
    GpioMonitor(GpioMonitor&&) = delete;
    GpioMonitor& operator=(GpioMonitor&&) = delete;

    /**
     * @brief Return whether periodic polling is currently active.
     */
    [[nodiscard]] bool isActive() const noexcept;

    /**
     * @brief Return the current polling interval.
     */
    [[nodiscard]]
    std::chrono::milliseconds interval() const noexcept;

    /**
     * @brief Set the periodic polling interval.
     *
     * @param interval Positive polling interval.
     */
    void setInterval(std::chrono::milliseconds interval);

    /**
     * @brief Return the maximum number of GPIO events collected per poll.
     */
    [[nodiscard]]
    std::size_t maxEventsPerPoll() const noexcept;

    /**
     * @brief Set the maximum number of events collected per poll.
     *
     * The underlying GpioPoller discards matching events beyond this limit.
     */
    void setMaxEventsPerPoll(std::size_t maxEvents);

    /**
     * @brief Replace the persistent GPIO edge filter.
     */
    void setFilter(const GpioEventFilter& filter);

    /**
     * @brief Clear the edge filter so all GPIO edges are accepted.
     */
    void clearFilter();

  public Q_SLOTS:
    /**
     * @brief Start periodic GPIO polling.
     *
     * Calling start() while already active has no effect.
     */
    void start();

    /**
     * @brief Stop periodic GPIO polling.
     *
     * Calling stop() while already stopped has no effect.
     */
    void stop();

    /**
     * @brief Perform one GPIO event poll immediately.
     *
     * Errors are reported through errorOccurred() and do not propagate as
     * exceptions from this slot.
     */
    void pollOnce();

  Q_SIGNALS:
    /**
     * @brief Emitted when periodic polling becomes active.
     */
    void started();

    /**
     * @brief Emitted when periodic polling stops.
     */
    void stopped();

    /**
     * @brief Emitted for every GPIO event reported by the core poller.
     */
    void gpioEvent(const libeasymcp2221::GpioEvent& event);

    /**
     * @brief Convenience signal emitted for rising GPIO edges.
     */
    void risingEdge(libeasymcp2221::Pin pin);

    /**
     * @brief Convenience signal emitted for falling GPIO edges.
     */
    void fallingEdge(libeasymcp2221::Pin pin);

    /**
     * @brief Emitted when a polling operation fails.
     *
     * A polling error does not automatically stop periodic monitoring.
     */
    void errorOccurred(
        const libeasymcp2221::qt::ErrorInfo& error);

  private:
    QTimer timer_;
    GpioPoller poller_;

    std::size_t maxEventsPerPoll_ = 4;
};

}  // namespace libeasymcp2221::qt

#endif  // LIBEASYMCP2221_CPP_QT_GPIO_MONITOR_H