#include <libeasymcp2221++/qt/GpioMonitor.h>

#include <limits>
#include <stdexcept>
#include <utility>

namespace libeasymcp2221::qt {

GpioMonitor::GpioMonitor(GpioPoller poller, QObject* parent) : QObject(parent),
poller_(std::move(poller)) {
	timer_.setParent(this);
	timer_.setInterval(static_cast<int>(DefaultInterval.count()));

	connect(&timer_, &QTimer::timeout, this, &GpioMonitor::pollOnce);
}

GpioMonitor::~GpioMonitor() = default;

bool GpioMonitor::isActive() const noexcept {
	return timer_.isActive();
}

std::chrono::milliseconds GpioMonitor::interval() const noexcept {
	return std::chrono::milliseconds{timer_.interval()};
}

void GpioMonitor::setInterval(std::chrono::milliseconds interval) {
	if (interval.count() <= 0) {
		throw std::invalid_argument("GPIO polling interval must be positive");
	}

	if (interval.count() > std::numeric_limits<int>::max()) {
		throw std::invalid_argument("GPIO polling interval is too large");
	}

	timer_.setInterval(static_cast<int>(interval.count()));
}

std::size_t GpioMonitor::maxEventsPerPoll() const noexcept {
	return maxEventsPerPoll_;
}

void GpioMonitor::setMaxEventsPerPoll(std::size_t maxEvents) {
	maxEventsPerPoll_ = maxEvents;
}

void GpioMonitor::setFilter(const GpioEventFilter& filter) {
	poller_.setFilter(filter);
}

void GpioMonitor::clearFilter() {
	poller_.clearFilter();
}

void GpioMonitor::start() {
	if (timer_.isActive()) {
		return;
	}

	timer_.start();
	emit started();
}

void GpioMonitor::stop() {
	if (!timer_.isActive()) {
		return;
	}

	timer_.stop();
	emit stopped();
}

void GpioMonitor::pollOnce() {
	try {
		const auto events = poller_.pollEvents(maxEventsPerPoll_);

		for (const auto& event : events) {
			emit gpioEvent(event);

			switch (event.edge) {
				case GpioEdge::Rising:
					emit risingEdge(event.pin);
					break;

				case GpioEdge::Falling:
					emit fallingEdge(event.pin);
					break;
			}
		}
	} catch (const libeasymcp2221::Error& error) {
		emit errorOccurred(toErrorInfo(error));
	} catch (const std::exception& error) {
		emit errorOccurred({libeasymcp2221::ErrorCode::Generic, 0, QString::fromUtf8(error.what())});
	}
}

}  // namespace libeasymcp2221::qt