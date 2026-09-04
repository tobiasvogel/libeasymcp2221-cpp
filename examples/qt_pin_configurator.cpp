#include <libeasymcp2221++/Device.h>
#include <libeasymcp2221++/Error.h>
#include <libeasymcp2221++/Types.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
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

static void addFunction(QComboBox* box, const char* name, PinFunction function) {
	box->addItem(name, static_cast<int>(function));
}

static void populateFunctions(QComboBox* box, Pin pin) {
	addFunction(box, "GPIO input", PinFunction::GpioInput);
	addFunction(box, "GPIO output", PinFunction::GpioOutput);

	switch (pin) {
		case Pin::GP0:
			addFunction(box, "SSPND", PinFunction::Dedicated);
			addFunction(box, "LED URx", PinFunction::Alt0);
			break;

		case Pin::GP1:
			addFunction(box, "CLKR", PinFunction::Dedicated);
			addFunction(box, "ADC1", PinFunction::Alt0);
			addFunction(box, "LED UTx", PinFunction::Alt1);
			addFunction(box, "IOC", PinFunction::Alt2);
			break;

		case Pin::GP2:
			addFunction(box, "USBCFG", PinFunction::Dedicated);
			addFunction(box, "ADC2", PinFunction::Alt0);
			addFunction(box, "DAC1", PinFunction::Alt1);
			break;

		case Pin::GP3:
			addFunction(box, "LED I2C", PinFunction::Dedicated);
			addFunction(box, "ADC3", PinFunction::Alt0);
			addFunction(box, "DAC2", PinFunction::Alt1);
			break;
	}
}

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);

	try {
		Device device;

		QWidget window;
		window.setWindowTitle("MCP2221 Pin Configurator");

		auto* layout = new QVBoxLayout(&window);
		auto* grid = new QGridLayout;
		auto* apply = new QPushButton("Apply");
		auto* status = new QLabel("Ready");

		layout->addLayout(grid);
		layout->addWidget(apply);
		layout->addWidget(status);

		grid->addWidget(new QLabel("Pin"), 0, 0);
		grid->addWidget(new QLabel("Function"), 0, 1);
		grid->addWidget(new QLabel("Output high"), 0, 2);

		std::array<QComboBox*, 4> functions{};
		std::array<QCheckBox*, 4> outputs{};

		for (std::size_t i = 0; i < 4; ++i) {
			auto* function = new QComboBox;
			auto* output = new QCheckBox;

			functions[i] = function;
			outputs[i] = output;

			populateFunctions(function, static_cast<Pin>(i));

			output->setEnabled(false);

			grid->addWidget(new QLabel(QString("GP%1").arg(i)), static_cast<int>(i + 1), 0);
			grid->addWidget(function, static_cast<int>(i + 1), 1);
			grid->addWidget(output, static_cast<int>(i + 1), 2);

			QObject::connect(function, &QComboBox::currentIndexChanged, [function, output] {
				const auto selected = static_cast<PinFunction>(function->currentData().toInt());

				const bool isOutput = selected == PinFunction::GpioOutput;

				output->setEnabled(isOutput);

				if (!isOutput)
					output->setChecked(false);
			});
		}

		QObject::connect(apply, &QPushButton::clicked, [&] {
			PinConfigurations configuration{};

			for (std::size_t i = 0; i < 4; ++i) {
				configuration[i].function = static_cast<PinFunction>(functions[i]->currentData().toInt());

				configuration[i].outputValue = outputs[i]->isChecked();
			}

			try {
				device.configurePins(configuration);
				status->setText("Configuration applied");
			} catch (const Error& error) {
				status->setText("Error: " + QString::fromUtf8(error.what()));
			}
		});

		window.show();
		return app.exec();
	} catch (const Error& error) {
		QMessageBox::critical(nullptr, "MCP2221 error", QString::fromUtf8(error.what()));
		return 1;
	}
}