#include <libeasymcp2221++/Device.h>
#include <libeasymcp2221++/Error.h>
#include <libeasymcp2221++/Types.h>

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSlider>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <Qt>

#include <cstdint>

using namespace libeasymcp2221;

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);

	try {
		Device device;

		/*
		 * Use GP1 and GP2 as ADC inputs and GP3 as DAC output.
		 * GP0 is left unchanged.
		 */
		PinConfigurations pins{};
		pins[1].function = PinFunction::Alt0;  // ADC1
		pins[2].function = PinFunction::Alt0;  // ADC2
		pins[3].function = PinFunction::Alt1;  // DAC2
		device.configurePins(pins);

		device.configureAdc(VoltageReference::Vdd);
		device.configureDac(VoltageReference::Vdd, 0);

		QWidget window;
		window.setWindowTitle("MCP2221 Analog Lab");

		auto* layout = new QVBoxLayout(&window);
		auto* grid = new QGridLayout;
		auto* adc1 = new QLabel("-");
		auto* adc2 = new QLabel("-");
		auto* dacValue = new QLabel("0");
		auto* dac = new QSlider(Qt::Horizontal);
		auto* status = new QLabel("Ready");

		dac->setRange(0, 31);
		dac->setValue(0);

		grid->addWidget(new QLabel("ADC1 (GP1)"), 0, 0);
		grid->addWidget(adc1, 0, 1);
		grid->addWidget(new QLabel("ADC2 (GP2)"), 1, 0);
		grid->addWidget(adc2, 1, 1);
		grid->addWidget(new QLabel("DAC2 (GP3)"), 2, 0);
		grid->addWidget(dac, 2, 1);
		grid->addWidget(dacValue, 2, 2);

		layout->addLayout(grid);
		layout->addWidget(status);

		QObject::connect(dac, &QSlider::valueChanged, [&](int value) {
			dacValue->setText(QString::number(value));

			try {
				device.writeDacRaw(static_cast<std::uint8_t>(value));
				status->setText("Ready");
			} catch (const Error& error) {
				status->setText("DAC error: " + QString::fromUtf8(error.what()));
			}
		});

		QTimer timer;
		timer.setInterval(250);

		QObject::connect(&timer, &QTimer::timeout, [&] {
			try {
				const auto values = device.readAdcRaw();

				adc1->setText(QString::number(values[0]));
				adc2->setText(QString::number(values[1]));
				status->setText("Ready");
			} catch (const Error& error) {
				status->setText("ADC error: " + QString::fromUtf8(error.what()));
			}
		});

		timer.start();

		window.show();
		return app.exec();
	} catch (const Error& error) {
		QMessageBox::critical(nullptr, "MCP2221 error", QString::fromUtf8(error.what()));
		return 1;
	}
}