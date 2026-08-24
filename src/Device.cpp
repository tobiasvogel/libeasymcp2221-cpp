/**
 * @file Device.cpp
 * @brief Device implementation and initial API stubs.
 */

#include "libeasymcp2221/Device.h"

#include <stdexcept>
#include <string>
#include <utility>

#include "libeasymcp2221/GpioPoller.h"
#include "libeasymcp2221/I2cDevice.h"
#include "libeasymcp2221/SmbusDevice.h"
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

void Device::setVdd(double)
{
    notImplemented("Device::setVdd");
}

double Device::vdd() const
{
    notImplemented("Device::vdd");
}

void Device::configureAdc(VoltageReference)
{
    notImplemented("Device::configureAdc");
}

std::array<std::uint16_t, 3> Device::readAdcRaw()
{
    notImplemented("Device::readAdcRaw");
}

std::array<double, 3> Device::readAdcNormalized()
{
    notImplemented("Device::readAdcNormalized");
}

std::array<double, 3> Device::readAdcVolts()
{
    notImplemented("Device::readAdcVolts");
}

void Device::configureDac(VoltageReference)
{
    notImplemented("Device::configureDac");
}

void Device::configureDac(VoltageReference, std::uint8_t)
{
    notImplemented("Device::configureDac");
}

void Device::writeDacRaw(std::uint8_t)
{
    notImplemented("Device::writeDacRaw");
}

void Device::writeDacNormalized(double)
{
    notImplemented("Device::writeDacNormalized");
}

void Device::writeDacVolts(double)
{
    notImplemented("Device::writeDacVolts");
}

void Device::configureClock(ClockDutyCycle, ClockFrequency)
{
    notImplemented("Device::configureClock");
}

bool Device::interruptFlag()
{
    notImplemented("Device::interruptFlag");
}

void Device::clearInterruptFlag()
{
    notImplemented("Device::clearInterruptFlag");
}

void Device::configureInterrupt(InterruptEdge)
{
    notImplemented("Device::configureInterrupt");
}

void Device::configureSram(const SramConfig&)
{
    notImplemented("Device::configureSram");
}

FlashData Device::readFlash(FlashSection)
{
    notImplemented("Device::readFlash");
}

void Device::writeFlash(FlashSection, const FlashData&)
{
    notImplemented("Device::writeFlash");
}

void Device::sendFlashPassword(const FlashPassword&)
{
    notImplemented("Device::sendFlashPassword");
}

FlashInfo Device::flashInfo()
{
    notImplemented("Device::flashInfo");
}

FlashSettings Device::flashSettings()
{
    notImplemented("Device::flashSettings");
}

void Device::saveConfigurationToFlash()
{
    notImplemented("Device::saveConfigurationToFlash");
}

void Device::stageUsbRemoteWakeup(bool)
{
    notImplemented("Device::stageUsbRemoteWakeup");
}

bool Device::usbRemoteWakeup()
{
    notImplemented("Device::usbRemoteWakeup");
}

void Device::stageUsbSelfPowered(bool)
{
    notImplemented("Device::stageUsbSelfPowered");
}

bool Device::usbSelfPowered()
{
    notImplemented("Device::usbSelfPowered");
}

void Device::stageUsbRequestedCurrent(unsigned)
{
    notImplemented("Device::stageUsbRequestedCurrent");
}

unsigned Device::usbRequestedCurrent()
{
    notImplemented("Device::usbRequestedCurrent");
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
