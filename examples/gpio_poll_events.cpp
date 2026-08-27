#include <chrono>
#include <iostream>
#include <thread>

#include <libeasymcp2221++/libeasymcp2221++.h>

using namespace libeasymcp2221;

int main()
{
    try {
        Device device;
        device.setPinFunction(Pin::GP0, PinFunction::GpioInput);

        auto poller = device.gpioPoller();
        GpioEventFilter filter{};
        filter.rising[0] = true;
        poller.setFilter(filter);

        std::cout << "Waiting for GPIO0_RISE...\n";

        for (;;) {
            const auto events = poller.pollEvents(8);
            if (!events.empty()) {
                const auto& event = events.front();
                std::cout << event.id() << " at " << event.time
                          << " (last_time=" << event.previousTime << ")\n";
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        return 0;
    }
    catch (const Error& error) {
        std::cerr << "GPIO poll failed: " << error.what() << '\n';
        return 1;
    }
}
