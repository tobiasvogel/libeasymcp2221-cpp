/**
 * @file Device.cpp
 * @brief Device implementation and initial API stubs.
 */

#include "libeasymcp2221++/Device.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>

#include "libeasymcp2221++/GpioPoller.h"
#include "libeasymcp2221++/I2cDevice.h"
#include "libeasymcp2221++/SmbusDevice.h"
#include "detail/CheckError.h"
#include "detail/DeviceState.h"

namespace libeasymcp2221 {

namespace {

[[noreturn]] void notImplemented(const char* operation)
{
    throw std::logic_error(std::string(operation) + " is not implemented yet");
}

void requireOpen(const std::shared_ptr<detail::DeviceState>& state)
{
    if (!state) {
        throw std::logic_error("Device is closed");
    }
}

mcp2221_i2c_kind_t toNativeTransfer(I2cTransfer transfer, bool writing)
{
    switch (transfer) {
    case I2cTransfer::Normal:
        return MCP2221_I2C_KIND_NORMAL;
    case I2cTransfer::RepeatedStart:
        return MCP2221_I2C_KIND_REPEATED_START;
    case I2cTransfer::NoStop:
        if (writing) {
            return MCP2221_I2C_KIND_NO_STOP;
        }
        detail::throwInvalid("I2cTransfer::NoStop is valid only for I2C writes");
    }
    detail::throwInvalid("Unknown I2C transfer kind");
}

void validateI2cAddress(std::uint8_t address)
{
    if (address > constants::MaxI2cAddress) {
        detail::throwInvalid("I2C address must be a 7-bit address");
    }
}

void validateI2cTransferBuffer(const std::uint8_t* data, std::size_t size)
{
    if (data == nullptr) {
        detail::throwInvalid("I2C data pointer must not be null");
    }
    if (size == 0 || size > constants::MaxI2cTransfer) {
        detail::throwInvalid("I2C transfer size must be from 1 through 65535 bytes");
    }
}

mcp2221_gpio_pin_t toNativePin(Pin pin)
{
    switch (pin) {
    case Pin::GP0: return MCP2221_GPIO_PIN_GP0;
    case Pin::GP1: return MCP2221_GPIO_PIN_GP1;
    case Pin::GP2: return MCP2221_GPIO_PIN_GP2;
    case Pin::GP3: return MCP2221_GPIO_PIN_GP3;
    }

    detail::throwInvalid("Unknown GPIO pin");
}

mcp2221_pin_function_t toNativePinFunction(PinFunction function)
{
    switch (function) {
    case PinFunction::Dedicated: return MCP2221_PIN_FUNC_DEDICATED;
    case PinFunction::Alt0: return MCP2221_PIN_FUNC_ALT0;
    case PinFunction::Alt1: return MCP2221_PIN_FUNC_ALT1;
    case PinFunction::Alt2: return MCP2221_PIN_FUNC_ALT2;
    case PinFunction::GpioInput: return MCP2221_PIN_FUNC_GPIO_IN;
    case PinFunction::GpioOutput: return MCP2221_PIN_FUNC_GPIO_OUT;
    }

    detail::throwInvalid("Unknown GPIO pin function");
}

int toNativeSramDirection(GpioDirection direction)
{
    switch (direction) {
    case GpioDirection::Output: return MCP2221_DIR_OUTPUT;
    case GpioDirection::Input: return MCP2221_DIR_INPUT;
    }

    detail::throwInvalid("Unknown GPIO direction");
}

int toNativeSramPinFunction(SramPinFunction function)
{
    switch (function) {
    case SramPinFunction::Gpio: return MCP2221_GPIO_FUNC_GPIO;
    case SramPinFunction::Dedicated: return MCP2221_GPIO_FUNC_DEDICATED;
    case SramPinFunction::Alt0: return MCP2221_GPIO_FUNC_ALT_0;
    case SramPinFunction::Alt1: return MCP2221_GPIO_FUNC_ALT_1;
    case SramPinFunction::Alt2: return MCP2221_GPIO_FUNC_ALT_2;
    }

    detail::throwInvalid("Unknown SRAM pin function");
}

void applyAdcReference(
    VoltageReference reference,
    mcp2221_sram_adc_config_t& native)
{
    switch (reference) {
    case VoltageReference::Off:
        native.vrm = MCP2221_ADC_VRM_OFF;
        native.ref_src = MCP2221_ADC_REF_VRM;
        return;
    case VoltageReference::Vdd:
        native.vrm = MCP2221_CONFIG_KEEP;
        native.ref_src = MCP2221_ADC_REF_VDD;
        return;
    case VoltageReference::Internal1V024:
        native.vrm = MCP2221_ADC_VRM_1024;
        native.ref_src = MCP2221_ADC_REF_VRM;
        return;
    case VoltageReference::Internal2V048:
        native.vrm = MCP2221_ADC_VRM_2048;
        native.ref_src = MCP2221_ADC_REF_VRM;
        return;
    case VoltageReference::Internal4V096:
        native.vrm = MCP2221_ADC_VRM_4096;
        native.ref_src = MCP2221_ADC_REF_VRM;
        return;
    }

    detail::throwInvalid("Unknown ADC voltage reference");
}

void applyDacReference(
    VoltageReference reference,
    mcp2221_sram_dac_ref_config_t& native)
{
    switch (reference) {
    case VoltageReference::Off:
        native.vrm = MCP2221_DAC_VRM_OFF;
        native.ref_src = MCP2221_DAC_REF_VRM;
        return;
    case VoltageReference::Vdd:
        native.vrm = MCP2221_CONFIG_KEEP;
        native.ref_src = MCP2221_DAC_REF_VDD;
        return;
    case VoltageReference::Internal1V024:
        native.vrm = MCP2221_DAC_VRM_1024;
        native.ref_src = MCP2221_DAC_REF_VRM;
        return;
    case VoltageReference::Internal2V048:
        native.vrm = MCP2221_DAC_VRM_2048;
        native.ref_src = MCP2221_DAC_REF_VRM;
        return;
    case VoltageReference::Internal4V096:
        native.vrm = MCP2221_DAC_VRM_4096;
        native.ref_src = MCP2221_DAC_REF_VRM;
        return;
    }

    detail::throwInvalid("Unknown DAC voltage reference");
}

int toNativeClockFrequency(ClockFrequency frequency)
{
    switch (frequency) {
    case ClockFrequency::KHz375: return MCP2221_CLK_FREQ_375kHz;
    case ClockFrequency::KHz750: return MCP2221_CLK_FREQ_750kHz;
    case ClockFrequency::MHz1_5: return MCP2221_CLK_FREQ_1_5MHz;
    case ClockFrequency::MHz3: return MCP2221_CLK_FREQ_3MHz;
    case ClockFrequency::MHz6: return MCP2221_CLK_FREQ_6MHz;
    case ClockFrequency::MHz12: return MCP2221_CLK_FREQ_12MHz;
    case ClockFrequency::MHz24: return MCP2221_CLK_FREQ_24MHz;
    }

    detail::throwInvalid("Unknown clock frequency");
}

int toNativeClockDuty(ClockDutyCycle duty)
{
    switch (duty) {
    case ClockDutyCycle::Percent0: return MCP2221_CLK_DUTY_0;
    case ClockDutyCycle::Percent25: return MCP2221_CLK_DUTY_25;
    case ClockDutyCycle::Percent50: return MCP2221_CLK_DUTY_50;
    case ClockDutyCycle::Percent75: return MCP2221_CLK_DUTY_75;
    }

    detail::throwInvalid("Unknown clock duty cycle");
}

const char* toReferenceString(VoltageReference reference)
{
    switch (reference) {
    case VoltageReference::Off: return "OFF";
    case VoltageReference::Vdd: return "VDD";
    case VoltageReference::Internal1V024: return "1.024V";
    case VoltageReference::Internal2V048: return "2.048V";
    case VoltageReference::Internal4V096: return "4.096V";
    }

    detail::throwInvalid("Unknown voltage reference");
}

const char* toClockFrequencyString(ClockFrequency frequency)
{
    switch (frequency) {
    case ClockFrequency::KHz375: return "375kHz";
    case ClockFrequency::KHz750: return "750kHz";
    case ClockFrequency::MHz1_5: return "1.5MHz";
    case ClockFrequency::MHz3: return "3MHz";
    case ClockFrequency::MHz6: return "6MHz";
    case ClockFrequency::MHz12: return "12MHz";
    case ClockFrequency::MHz24: return "24MHz";
    }

    detail::throwInvalid("Unknown clock frequency");
}

int toDutyPercent(ClockDutyCycle duty)
{
    switch (duty) {
    case ClockDutyCycle::Percent0: return 0;
    case ClockDutyCycle::Percent25: return 25;
    case ClockDutyCycle::Percent50: return 50;
    case ClockDutyCycle::Percent75: return 75;
    }

    detail::throwInvalid("Unknown clock duty cycle");
}

const char* toInterruptEdgeString(InterruptEdge edge)
{
    switch (edge) {
    case InterruptEdge::None: return "none";
    case InterruptEdge::Rising: return "rising";
    case InterruptEdge::Falling: return "falling";
    case InterruptEdge::Both: return "both";
    }

    detail::throwInvalid("Unknown interrupt edge selection");
}

std::uint8_t toNativeFlashSection(FlashSection section)
{
    switch (section) {
    case FlashSection::ChipSettings:
        return MCP2221_FLASH_DATA_CHIP_SETTINGS;
    case FlashSection::GpSettings:
        return MCP2221_FLASH_DATA_GP_SETTINGS;
    case FlashSection::UsbManufacturer:
        return MCP2221_FLASH_DATA_USB_MANUFACTURER;
    case FlashSection::UsbProduct:
        return MCP2221_FLASH_DATA_USB_PRODUCT;
    case FlashSection::UsbSerial:
        return MCP2221_FLASH_DATA_USB_SERIALNUM;
    case FlashSection::ChipSerial:
        return MCP2221_FLASH_DATA_CHIP_SERIALNUM;
    }

    detail::throwInvalid("Unknown flash section");
}

} // namespace

Device::Device()
    : Device(DeviceOptions{})
{
}

Device::Device(const DeviceOptions& options)
{
    if (options.i2cSpeedHz == 0 ||
        options.i2cSpeedHz > constants::MaxI2cSpeedHz) {
        detail::throwInvalid("Initial I2C speed must be from 1 through 400000 Hz");
    }

    mcp2221_t* handle = nullptr;
    const char* serial =
        options.usbSerial.empty() ? nullptr : options.usbSerial.c_str();

    detail::checkError(
        mcp2221_open_scan(
            options.vendorId,
            options.productId,
            options.deviceIndex,
            serial,
            options.usbReadTimeoutMs,
            options.commandRetries,
            options.debugMessages ? 1 : 0,
            options.tracePackets ? 1 : 0,
            options.scanFlashSerial ? 1 : 0,
            &handle),
        "Opening MCP2221");

    /*
     * Own the acquired C reference immediately so every later initialization
     * failure releases it automatically.
     */
    auto state = std::make_shared<detail::DeviceState>(handle);

    {
        std::lock_guard<std::mutex> lock(state->mutex());

        /*
         * Mirror the public C API's open_simple_scan() initialization sequence:
         * stale I2C state is released best-effort, then a safe 100 kHz clock is
         * established before applying the requested target speed.
         */
        (void)mcp2221_i2c_release(state->handle());

        detail::checkError(
            mcp2221_i2c_set_speed(state->handle(), 100000),
            "Setting safe initial I2C speed");

        if (options.i2cSpeedHz != 100000) {
            detail::checkError(
                mcp2221_i2c_set_speed(state->handle(), options.i2cSpeedHz),
                "Setting requested I2C speed");
        }
    }

    state_ = std::move(state);
}

Device::~Device() noexcept = default;

void Device::close() noexcept
{
    state_.reset();
}

bool Device::isOpen() const noexcept
{
    return static_cast<bool>(state_);
}

I2cDevice Device::i2cDevice(std::uint8_t address)
{
    return i2cDevice(address, I2cDeviceOptions{});
}

I2cDevice Device::i2cDevice(
    std::uint8_t address,
    const I2cDeviceOptions& options)
{
    requireOpen(state_);
    validateI2cAddress(address);

    if (options.speedHz == 0 ||
        options.speedHz > constants::MaxI2cSpeedHz) {
        detail::throwInvalid("I2C target speed must be from 1 through 400000 Hz");
    }
    if (options.registerBytes < 1 || options.registerBytes > 4) {
        detail::throwInvalid("I2C target register width must be from 1 through 4 bytes");
    }

    auto child = std::make_shared<detail::I2cDeviceState>();
    child->device = state_;

    std::lock_guard<std::mutex> lock(state_->mutex());

    const auto order =
        options.byteOrder == ByteOrder::LittleEndian
            ? MCP2221_I2C_BYTE_ORDER_LITTLE
            : MCP2221_I2C_BYTE_ORDER_BIG;

    detail::checkError(
        mcp2221_i2c_slave_init(
            &child->slave,
            state_->handle(),
            address,
            options.force ? 1 : 0,
            options.speedHz,
            options.registerBytes,
            order),
        "Initializing I2C target");

    return I2cDevice(std::move(child));
}

SmbusDevice Device::smbusDevice(std::uint8_t address)
{
    requireOpen(state_);
    validateI2cAddress(address);

    auto child = std::make_shared<detail::SmbusDeviceState>();
    child->device = state_;
    child->address = address;

    std::lock_guard<std::mutex> lock(state_->mutex());

    detail::checkError(
        mcp2221_smbus_init(
            &child->bus,
            state_->handle(),
            0,
            0,
            0,
            nullptr,
            0),
        "Initializing SMBus target");

    return SmbusDevice(std::move(child));
}

GpioPoller Device::gpioPoller()
{
    if (!state_) {
        throw std::logic_error("Device is closed");
    }

    auto child = std::make_unique<detail::GpioPollerState>();
    child->device = state_;
    mcp2221_gpio_poll_init(&child->poll);

    return GpioPoller(std::move(child));
}

void Device::setI2cSpeed(std::uint32_t hz)
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_i2c_set_speed(state_->handle(), hz),
        "Setting I2C speed");
}

void Device::i2cWrite(
    std::uint8_t address,
    const std::uint8_t* data,
    std::size_t size,
    I2cTransfer transfer)
{
    requireOpen(state_);
    validateI2cAddress(address);
    validateI2cTransferBuffer(data, size);

    const auto nativeTransfer = toNativeTransfer(transfer, true);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_i2c_write_simple(
            state_->handle(),
            address,
            data,
            size,
            nativeTransfer),
        "Writing I2C data");
}

void Device::i2cWrite(
    std::uint8_t address,
    const std::vector<std::uint8_t>& data,
    I2cTransfer transfer)
{
    i2cWrite(address, data.data(), data.size(), transfer);
}

std::vector<std::uint8_t> Device::i2cRead(
    std::uint8_t address,
    std::size_t size,
    I2cTransfer transfer)
{
    requireOpen(state_);
    validateI2cAddress(address);

    if (size == 0 || size > constants::MaxI2cTransfer) {
        detail::throwInvalid("I2C read size must be from 1 through 65535 bytes");
    }

    const auto nativeTransfer = toNativeTransfer(transfer, false);
    std::vector<std::uint8_t> data(size);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_i2c_read_simple(
            state_->handle(),
            address,
            data.data(),
            data.size(),
            nativeTransfer),
        "Reading I2C data");

    return data;
}

I2cStatus Device::i2cStatus()
{
    requireOpen(state_);

    mcp2221_i2c_status_t native{};
    {
        std::lock_guard<std::mutex> lock(state_->mutex());
        detail::checkError(
            mcp2221_i2c_status(state_->handle(), &native),
            "Reading I2C status");
    }

    I2cStatus status;
    status.requestedLength = native.rlen;
    status.transmittedLength = native.txlen;
    status.divider = native.div;
    status.acknowledged = native.ack != 0;
    status.state = native.st;
    status.scl = native.scl != 0;
    status.sda = native.sda != 0;
    status.confused = native.confused != 0;
    status.initialized = native.initialized != 0;
    return status;
}

void Device::releaseI2c()
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_i2c_release(state_->handle()),
        "Releasing I2C engine");
}

GpioState Device::readGpio()
{
    requireOpen(state_);

    int nativeState[4] = {-1, -1, -1, -1};
    std::uint8_t validMask = 0;

    {
        std::lock_guard<std::mutex> lock(state_->mutex());
        detail::checkError(
            mcp2221_gpio_read_mask(
                state_->handle(),
                nativeState,
                &validMask),
            "Reading GPIO state");
    }

    GpioState state;
    for (std::size_t i = 0; i < state.pins.size(); ++i) {
        if ((validMask & static_cast<std::uint8_t>(1u << i)) == 0) {
            state.pins[i] = std::nullopt;
            continue;
        }

        if (nativeState[i] == 0) {
            state.pins[i] = false;
        }
        else if (nativeState[i] == 1) {
            state.pins[i] = true;
        }
        else {
            throw Error(
                ErrorCode::Protocol,
                static_cast<int>(MCP2221_ERR_PROTOCOL),
                "Reading GPIO state: invalid GPIO value returned by C library");
        }
    }

    return state;
}

void Device::writeGpio(const GpioWrite& values)
{
    requireOpen(state_);

    mcp2221_gpio_write_t request{
        MCP2221_GPIO_KEEP,
        MCP2221_GPIO_KEEP,
        MCP2221_GPIO_KEEP,
        MCP2221_GPIO_KEEP
    };

    int* nativeValues[4] = {
        &request.gp0,
        &request.gp1,
        &request.gp2,
        &request.gp3
    };

    for (std::size_t i = 0; i < values.pins.size(); ++i) {
        if (values.pins[i].has_value()) {
            *nativeValues[i] = *values.pins[i] ? 1 : 0;
        }
    }

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_gpio_write(state_->handle(), &request),
        "Writing GPIO state");
}

void Device::setPinFunction(Pin pin, PinFunction function)
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_pin_set_function(
            state_->handle(),
            toNativePin(pin),
            toNativePinFunction(function)),
        "Setting GPIO pin function");
}

void Device::configurePins(const PinConfigurations& configuration)
{
    requireOpen(state_);

    mcp2221_pin_functions_t native{};
    for (std::size_t i = 0; i < configuration.size(); ++i) {
        const auto& entry = configuration[i];

        if (!entry.function.has_value()) {
            if (entry.outputValue) {
                detail::throwInvalid(
                    "outputValue must be false when a pin function is preserved");
            }

            native.gp[i] = MCP2221_PIN_FUNC_KEEP;
            native.out[i] = 0;
            continue;
        }

        native.gp[i] = toNativePinFunction(*entry.function);

        if (*entry.function == PinFunction::GpioOutput) {
            native.out[i] = entry.outputValue ? 1 : 0;
        }
        else {
            if (entry.outputValue) {
                detail::throwInvalid(
                    "outputValue may be true only for PinFunction::GpioOutput");
            }
            native.out[i] = 0;
        }
    }

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_pin_set_functions(state_->handle(), &native),
        "Configuring GPIO pin functions");
}

void Device::setVdd(double volts)
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_analog_set_vdd(state_->handle(), volts),
        "Setting VDD");
}

double Device::vdd() const
{
    requireOpen(state_);

    double volts = 0.0;
    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_analog_get_vdd(state_->handle(), &volts),
        "Reading VDD");

    return volts;
}

void Device::configureAdc(VoltageReference reference)
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_adc_config(
            state_->handle(),
            toReferenceString(reference)),
        "Configuring ADC reference");
}

std::array<std::uint16_t, 3> Device::readAdcRaw()
{
    requireOpen(state_);

    std::array<std::uint16_t, 3> values{};
    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_adc_read_raw(state_->handle(), values.data()),
        "Reading raw ADC values");

    return values;
}

std::array<double, 3> Device::readAdcNormalized()
{
    requireOpen(state_);

    std::array<double, 3> values{};
    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_adc_read_normalized(state_->handle(), values.data()),
        "Reading normalized ADC values");

    return values;
}

std::array<double, 3> Device::readAdcVolts()
{
    requireOpen(state_);

    std::array<double, 3> values{};
    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_adc_read_volts(state_->handle(), values.data()),
        "Reading ADC voltages");

    return values;
}

void Device::configureDac(VoltageReference reference)
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_dac_config(
            state_->handle(),
            toReferenceString(reference)),
        "Configuring DAC reference");
}

void Device::configureDac(
    VoltageReference reference,
    std::uint8_t outputCode)
{
    requireOpen(state_);

    if (outputCode > 31) {
        detail::throwInvalid("DAC output code must be from 0 through 31");
    }

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_dac_config_out(
            state_->handle(),
            toReferenceString(reference),
            outputCode),
        "Configuring DAC reference and output");
}

void Device::writeDacRaw(std::uint8_t code)
{
    requireOpen(state_);

    if (code > 31) {
        detail::throwInvalid("DAC output code must be from 0 through 31");
    }

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_dac_write_raw(state_->handle(), code),
        "Writing raw DAC output");
}

void Device::writeDacNormalized(double value)
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_dac_write_normalized(state_->handle(), value),
        "Writing normalized DAC output");
}

void Device::writeDacVolts(double volts)
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_dac_write_volts(state_->handle(), volts),
        "Writing DAC voltage");
}

void Device::configureClock(
    ClockDutyCycle duty,
    ClockFrequency frequency)
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_clock_config(
            state_->handle(),
            toDutyPercent(duty),
            toClockFrequencyString(frequency)),
        "Configuring clock output");
}

bool Device::interruptFlag()
{
    requireOpen(state_);

    std::uint8_t flag = 0;
    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_ioc_read(state_->handle(), &flag),
        "Reading interrupt-on-change flag");

    return flag != 0;
}

void Device::clearInterruptFlag()
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_ioc_clear(state_->handle()),
        "Clearing interrupt-on-change flag");
}

void Device::configureInterrupt(InterruptEdge edge)
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_ioc_config(
            state_->handle(),
            toInterruptEdgeString(edge)),
        "Configuring interrupt-on-change");
}

void Device::configureSram(const SramConfig& configuration)
{
    requireOpen(state_);

    mcp2221_sram_config_t native{};

    for (std::size_t i = 0; i < configuration.gpio.size(); ++i) {
        const auto& pin = configuration.gpio[i];

        native.gp[i].value =
            pin.outputValue.has_value()
                ? (*pin.outputValue ? 1 : 0)
                : MCP2221_CONFIG_KEEP;

        native.gp[i].direction =
            pin.direction.has_value()
                ? toNativeSramDirection(*pin.direction)
                : MCP2221_CONFIG_KEEP;

        native.gp[i].function =
            pin.function.has_value()
                ? toNativeSramPinFunction(*pin.function)
                : MCP2221_CONFIG_KEEP;
    }

    native.int_cfg.pos_edge =
        configuration.interrupt.risingEdge.has_value()
            ? (*configuration.interrupt.risingEdge ? 1 : 0)
            : MCP2221_CONFIG_KEEP;

    native.int_cfg.neg_edge =
        configuration.interrupt.fallingEdge.has_value()
            ? (*configuration.interrupt.fallingEdge ? 1 : 0)
            : MCP2221_CONFIG_KEEP;

    native.int_cfg.clear_flag =
        configuration.interrupt.clearFlag.has_value()
            ? (*configuration.interrupt.clearFlag ? 1 : 0)
            : MCP2221_CONFIG_KEEP;

    native.adc_cfg.vrm = MCP2221_CONFIG_KEEP;
    native.adc_cfg.ref_src = MCP2221_CONFIG_KEEP;
    if (configuration.adc.reference.has_value()) {
        applyAdcReference(*configuration.adc.reference, native.adc_cfg);
    }

    native.dac_ref.vrm = MCP2221_CONFIG_KEEP;
    native.dac_ref.ref_src = MCP2221_CONFIG_KEEP;
    if (configuration.dac.reference.has_value()) {
        applyDacReference(*configuration.dac.reference, native.dac_ref);
    }

    native.dac_val.value = MCP2221_CONFIG_KEEP;
    if (configuration.dac.value.has_value()) {
        if (*configuration.dac.value > 31) {
            detail::throwInvalid("DAC SRAM value must be from 0 through 31");
        }
        native.dac_val.value = *configuration.dac.value;
    }

    native.clk_cfg.div =
        configuration.clock.frequency.has_value()
            ? toNativeClockFrequency(*configuration.clock.frequency)
            : MCP2221_CONFIG_KEEP;

    native.clk_cfg.duty =
        configuration.clock.dutyCycle.has_value()
            ? toNativeClockDuty(*configuration.clock.dutyCycle)
            : MCP2221_CONFIG_KEEP;

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_sram_config(state_->handle(), &native),
        "Configuring runtime SRAM");
}

FlashData Device::readFlash(FlashSection section)
{
    requireOpen(state_);

    FlashData data{};
    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_flash_read(
            state_->handle(),
            toNativeFlashSection(section),
            data.data()),
        "Reading flash section");

    return data;
}

void Device::writeFlash(
    FlashSection section,
    const FlashData& data)
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_flash_write(
            state_->handle(),
            toNativeFlashSection(section),
            data.data()),
        "Writing flash section");
}

void Device::sendFlashPassword(const FlashPassword& password)
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_flash_send_password(
            state_->handle(),
            password.data()),
        "Sending flash access password");
}

FlashInfo Device::flashInfo()
{
    requireOpen(state_);

    mcp2221_flash_info_t native{};
    {
        std::lock_guard<std::mutex> lock(state_->mutex());
        detail::checkError(
            mcp2221_flash_read_info(
                state_->handle(),
                &native),
            "Reading flash information");
    }

    FlashInfo info;
    std::copy(
        std::begin(native.chip_settings),
        std::end(native.chip_settings),
        info.chipSettings.begin());
    std::copy(
        std::begin(native.gp_settings),
        std::end(native.gp_settings),
        info.gpSettings.begin());
    std::copy(
        std::begin(native.usb_manufacturer),
        std::end(native.usb_manufacturer),
        info.usbManufacturerRaw.begin());
    std::copy(
        std::begin(native.usb_product),
        std::end(native.usb_product),
        info.usbProductRaw.begin());
    std::copy(
        std::begin(native.usb_serial),
        std::end(native.usb_serial),
        info.usbSerialRaw.begin());
    std::copy(
        std::begin(native.usb_factory_serial),
        std::end(native.usb_factory_serial),
        info.factorySerialRaw.begin());

    info.usbManufacturer = native.usb_manufacturer_str;
    info.usbProduct = native.usb_product_str;
    info.usbSerial = native.usb_serial_str;
    info.factorySerial = native.usb_factory_serial_str;

    return info;
}

FlashSettings Device::flashSettings()
{
    requireOpen(state_);

    mcp2221_flash_settings_t native{};
    {
        std::lock_guard<std::mutex> lock(state_->mutex());
        detail::checkError(
            mcp2221_flash_get_settings(
                state_->handle(),
                &native),
            "Reading flash settings");
    }

    FlashSettings settings;
    std::copy(
        std::begin(native.chip_settings),
        std::end(native.chip_settings),
        settings.chip.begin());
    std::copy(
        std::begin(native.gp_settings),
        std::end(native.gp_settings),
        settings.gpio.begin());

    return settings;
}

void Device::saveConfigurationToFlash()
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_flash_save_config(state_->handle()),
        "Saving configuration to flash");
}

void Device::stageUsbRemoteWakeup(bool enabled)
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_usb_set_remote_wakeup(
            state_->handle(),
            enabled ? 1 : 0),
        "Staging USB Remote Wake-up");
}

bool Device::usbRemoteWakeup()
{
    requireOpen(state_);

    int enabled = 0;
    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_usb_get_remote_wakeup(
            state_->handle(),
            &enabled),
        "Reading USB Remote Wake-up setting");

    return enabled != 0;
}

void Device::stageUsbSelfPowered(bool enabled)
{
    requireOpen(state_);

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_usb_set_self_powered(
            state_->handle(),
            enabled ? 1 : 0),
        "Staging USB self-powered setting");
}

bool Device::usbSelfPowered()
{
    requireOpen(state_);

    int enabled = 0;
    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_usb_get_self_powered(
            state_->handle(),
            &enabled),
        "Reading USB self-powered setting");

    return enabled != 0;
}

void Device::stageUsbRequestedCurrent(unsigned milliamps)
{
    requireOpen(state_);

    if (milliamps > constants::MaxUsbCurrentMa ||
        (milliamps % 2u) != 0u) {
        detail::throwInvalid(
            "USB requested current must be an even value from 0 through 500 mA");
    }

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_usb_set_requested_current(
            state_->handle(),
            milliamps),
        "Staging USB requested current");
}

unsigned Device::usbRequestedCurrent()
{
    requireOpen(state_);

    unsigned milliamps = 0;
    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_usb_get_requested_current(
            state_->handle(),
            &milliamps),
        "Reading USB requested current");

    return milliamps;
}

std::array<std::uint8_t, 64> Device::rawCommand(
    const std::uint8_t* command,
    std::size_t size)
{
    requireOpen(state_);

    if (command == nullptr) {
        detail::throwInvalid("Raw command pointer must not be null");
    }
    if (size == 0 || size > constants::PacketSize) {
        detail::throwInvalid("Raw command size must be from 1 through 64 bytes");
    }

    std::array<std::uint8_t, 64> response{};

    std::lock_guard<std::mutex> lock(state_->mutex());
    detail::checkError(
        mcp2221_send_cmd(state_->handle(), command, size, response.data()),
        "Sending raw MCP2221 command");

    return response;
}

} // namespace libeasymcp2221
