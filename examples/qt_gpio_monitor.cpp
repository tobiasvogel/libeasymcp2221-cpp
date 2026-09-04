#include <libeasymcp2221++/Device.h>
#include <libeasymcp2221++/Error.h>
#include <libeasymcp2221++/Types.h>
#include <libeasymcp2221++/qt/GpioMonitor.h>

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <array>
#include <cstddef>

using namespace libeasymcp2221;

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);

	try {
		Device device;

		PinConfigurations pins{};
		for (auto& pin : pins)
			pin.function = PinFunction::GpioInput;
		device.configurePins(pins);

		libeasymcp2221::qt::GpioMonitor monitor(device.gpioPoller());

		QWidget window;
		window.setWindowTitle("MCP2221 GPIO Monitor");

		auto* layout = new QVBoxLayout(&window);
		auto* grid = new QGridLayout;
		auto* button = new QPushButton("Start");
		auto* status = new QLabel("Stopped");
		auto* lastEvent = new QLabel("Last event: none");

		layout->addLayout(grid);
		layout->addWidget(lastEvent);
		layout->addWidget(status);
		layout->addWidget(button);

		grid->addWidget(new QLabel("Pin"), 0, 0);
		grid->addWidget(new QLabel("Rising"), 0, 1);
		grid->addWidget(new QLabel("Falling"), 0, 2);

		std::array<QLabel*, 4> risingLabels{};
		std::array<QLabel*, 4> fallingLabels{};
		std::array<unsigned long, 4> risingCounts{};
		std::array<unsigned long, 4> fallingCounts{};

		for (std::size_t i = 0; i < 4; ++i) {
			grid->addWidget(new QLabel(QString("GP%1").arg(i)), static_cast<int>(i + 1), 0);

			risingLabels[i] = new QLabel("0");
			fallingLabels[i] = new QLabel("0");

			grid->addWidget(risingLabels[i], static_cast<int>(i + 1), 1);
			grid->addWidget(fallingLabels[i], static_cast<int>(i + 1), 2);
		}

		QObject::connect(button, &QPushButton::clicked, [&monitor] {
			if (monitor.isActive())
				monitor.stop();
			else
				monitor.start();
		});

		QObject::connect(&monitor, &libeasymcp2221::qt::GpioMonitor::started, [&] {
			button->setText("Stop");
			status->setText("Monitoring GPIO edges");
		});

		QObject::connect(&monitor, &libeasymcp2221::qt::GpioMonitor::stopped, [&] {
			button->setText("Start");
			status->setText("Stopped");
		});

		QObject::connect(&monitor, &libeasymcp2221::qt::GpioMonitor::risingEdge, [&](Pin pin) {
			const auto index = static_cast<std::size_t>(pin);
			risingLabels[index]->setText(QString::number(++risingCounts[index]));
		});

		QObject::connect(&monitor, &libeasymcp2221::qt::GpioMonitor::fallingEdge, [&](Pin pin) {
			const auto index = static_cast<std::size_t>(pin);
			fallingLabels[index]->setText(QString::number(++fallingCounts[index]));
		});

		QObject::connect(&monitor, &libeasymcp2221::qt::GpioMonitor::gpioEvent, [&](const GpioEvent& event) {
			lastEvent->setText("Last event: " + QString::fromStdString(event.id()));
		});

		QObject::connect(
			&monitor, &libeasymcp2221::qt::GpioMonitor::errorOccurred,
			[&](const libeasymcp2221::qt::ErrorInfo& error) { status->setText("Error: " + error.message); });

		window.show();
		return app.exec();
	} catch (const Error& error) {
		QMessageBox::critical(nullptr, "MCP2221 error", QString::fromUtf8(error.what()));
		return 1;
	}
}