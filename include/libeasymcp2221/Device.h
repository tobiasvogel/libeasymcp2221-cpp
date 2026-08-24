/**
 * @file Device.h
 * @brief Root RAII object representing an MCP2221(A).
 */

#ifndef LIBEASYMCP2221_CPP_DEVICE_H
#define LIBEASYMCP2221_CPP_DEVICE_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "DeviceOptions.h"
#include "Types.h"

namespace libeasymcp2221 {

class GpioPoller;
class I2cDevice;
class SmbusDevice;
struct I2cDeviceOptions;

namespace detail {
class DeviceState;
}

/**
 * @brief Root object representing one opened MCP2221(A).
 *
 * Device is move-only. Child adapters retain a shared internal device state,
 * so an I2cDevice or SmbusDevice can safely keep the underlying C handle alive
 * after the originating Device object itself has moved or gone out of scope.
 *
 * Destruction releases host/library resources only. It does not intentionally
 * reset GPIO, DAC, SRAM, flash, or other externally visible hardware state.
 */
class Device {
public:
    /**
     * @brief Open the first default MCP2221(A) using default options.
     * @throws Error if the device cannot be opened or initialized.
     */
    Device();

    /**
     * @brief Open an MCP2221(A) using explicit options.
     * @param options Device-selection and initialization options.
     * @throws Error if the device cannot be opened or initialized.
     */
    explicit Device(const DeviceOptions& options);

    /** @brief Release this Device object's reference to the shared device state. */
    ~Device() noexcept;

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    Device(Device&&) noexcept = default;
    Device& operator=(Device&&) noexcept = default;

    /**
     * @brief Explicitly release this Device object's reference.
     *
     * Existing child objects may keep the underlying MCP2221 handle alive.
     * Calling close() more than once is safe.
     */
    void close() noexcept;

    /**
     * @brief Check whether this Device object currently references device state.
     */
    bool isOpen() const noexcept;

    /**
     * @brief Create a stateful adapter for one I2C target using default options.
     * @param address 7-bit I2C target address.
     * @return Stateful I2C target adapter.
     */
    I2cDevice i2cDevice(std::uint8_t address);

    /**
     * @brief Create a stateful adapter for one I2C target.
     * @param address 7-bit I2C target address.
     * @param options Target-specific adapter options.
     * @return Stateful I2C target adapter.
     */
    I2cDevice i2cDevice(
        std::uint8_t address,
        const I2cDeviceOptions& options);

    /** @brief Create a stateful adapter for one SMBus target. */
    SmbusDevice smbusDevice(std::uint8_t address);

    /** @brief Create a move-only stateful GPIO poller. */
    GpioPoller gpioPoller();

    /** @brief Set the MCP2221 I2C bus clock frequency. */
    void setI2cSpeed(std::uint32_t hz);

    /** @brief Perform a raw I2C write using the default transfer watchdog. */
    void i2cWrite(
        std::uint8_t address,
        const std::uint8_t* data,
        std::size_t size,
        I2cTransfer transfer = I2cTransfer::Normal);

    /** @brief Convenience overload for std::vector payloads. */
    void i2cWrite(
        std::uint8_t address,
        const std::vector<std::uint8_t>& data,
        I2cTransfer transfer = I2cTransfer::Normal);

    /** @brief Perform a raw I2C read using the default transfer watchdog. */
    std::vector<std::uint8_t> i2cRead(
        std::uint8_t address,
        std::size_t size,
        I2cTransfer transfer = I2cTransfer::Normal);

    /** @brief Return a typed I2C-engine status snapshot. */
    I2cStatus i2cStatus();

    /** @brief Release/reset the MCP2221 I2C engine. */
    void releaseI2c();

    /**
     * @brief Read the current logical GPIO states.
     *
     * Pins that are not configured as GPIO are returned as std::nullopt.
     *
     * @return Logical state for GP0 through GP3.
     * @throws Error on transport or protocol failure.
     */
    GpioState readGpio();

    /**
     * @brief Apply a partial GPIO output update.
     *
     * Disengaged std::optional entries preserve the corresponding output.
     *
     * @warning The underlying MCP2221 GPIO command can partially succeed.
     *          If ErrorCode::GpioMode is thrown, other requested GPIO pins may
     *          already have been updated successfully.
     *
     * @param values Partial GP0-through-GP3 output update.
     * @throws Error on invalid mode, protocol, or transport failure.
     */
    void writeGpio(const GpioWrite& values);

    /**
     * @brief Configure one GP pin function.
     *
     * GPIO outputs configured through this single-pin helper are initialized
     * low, matching the underlying C API.
     *
     * @param pin GP pin to configure.
     * @param function Requested pin function.
     * @throws Error if the function is unsupported for the selected pin or the
     *         device operation fails.
     */
    void setPinFunction(Pin pin, PinFunction function);

    /**
     * @brief Apply a partial four-pin batch configuration.
     *
     * Entries with a disengaged function preserve the corresponding pin.
     * For GpioOutput entries, outputValue selects the initial output level.
     * For every other function outputValue must be false.
     *
     * @param configuration GP0-through-GP3 configuration.
     * @throws Error for unsupported pin/function combinations or device
     *         operation failures.
     */
    void configurePins(const PinConfigurations& configuration);

    /**
     * @brief Store the externally supplied MCP2221 VDD value.
     *
     * The value is retained by the C device context and is used for ADC/DAC
     * voltage conversions whenever VDD is the selected reference.
     *
     * @param volts Supply voltage in volts.
     * @throws Error if the value is outside the supported range.
     */
    void setVdd(double volts);

    /**
     * @brief Return the currently stored MCP2221 VDD value.
     * @return Previously configured supply voltage.
     * @throws Error if no VDD value has been configured.
     */
    double vdd() const;

    /**
     * @brief Configure the ADC voltage reference.
     * @param reference Strongly typed ADC reference selection.
     * @throws Error on invalid configuration or device failure.
     */
    void configureAdc(VoltageReference reference);

    /** @brief Read the three ADC channels as raw 10-bit values. */
    std::array<std::uint16_t, 3> readAdcRaw();

    /** @brief Read the three ADC channels as normalized values. */
    std::array<double, 3> readAdcNormalized();

    /** @brief Read the three ADC channels as voltages. */
    std::array<double, 3> readAdcVolts();

    /**
     * @brief Configure the DAC voltage reference while preserving output code.
     *
     * The underlying C helper retains the EasyMCP2221 workaround for MCP2221
     * reference-transition quirks.
     */
    void configureDac(VoltageReference reference);

    /**
     * @brief Configure the DAC reference and raw 5-bit output code.
     * @param reference Strongly typed DAC reference selection.
     * @param outputCode Raw DAC code from 0 through 31.
     */
    void configureDac(VoltageReference reference, std::uint8_t outputCode);

    /** @brief Write a raw 5-bit DAC code. */
    void writeDacRaw(std::uint8_t code);

    /** @brief Write a normalized DAC output value. */
    void writeDacNormalized(double value);

    /** @brief Write a DAC output voltage. */
    void writeDacVolts(double volts);

    /**
     * @brief Configure the MCP2221 clock output.
     * @param duty Clock duty-cycle selection.
     * @param frequency Clock-output frequency selection.
     */
    void configureClock(ClockDutyCycle duty, ClockFrequency frequency);

    /** @brief Read the interrupt-on-change flag. */
    bool interruptFlag();

    /** @brief Clear the interrupt-on-change flag. */
    void clearInterruptFlag();

    /**
     * @brief Configure interrupt-on-change edge detection.
     * @param edge Edge-detection mode.
     */
    void configureInterrupt(InterruptEdge edge);

    /**
     * @brief Apply a strongly typed partial runtime SRAM configuration.
     *
     * Every disengaged std::optional in @p configuration preserves the current
     * device field. The C++ value types are translated to the native
     * mcp2221_sram_config_t representation and the C library remains
     * responsible for device-specific merge/validation behavior.
     *
     * @param configuration Partial GPIO, IOC, ADC, DAC, and clock settings.
     * @throws Error on invalid values, unsupported pin/function combinations,
     *         transport failures, or protocol failures.
     *
     * @note This method changes runtime SRAM only; it does not write flash.
     */
    void configureSram(const SramConfig& configuration);

    /** @brief Read one raw persistent flash section. */
    FlashData readFlash(FlashSection section);

    /** @brief Write one raw persistent flash section. */
    void writeFlash(FlashSection section, const FlashData& data);

    /** @brief Send the eight-byte flash access password. */
    void sendFlashPassword(const FlashPassword& password);

    /** @brief Read aggregate persistent flash information. */
    FlashInfo flashInfo();

    /** @brief Read raw persistent chip and GPIO flash settings. */
    FlashSettings flashSettings();

    /**
     * @brief Persist the current staged/runtime configuration to flash.
     *
     * This operation performs persistent hardware writes.
     */
    void saveConfigurationToFlash();

    /** @brief Stage the USB Remote Wake-up capability value. */
    void stageUsbRemoteWakeup(bool enabled);

    /** @brief Return the effective staged/persisted USB Remote Wake-up value. */
    bool usbRemoteWakeup();

    /** @brief Stage the USB self-powered enumeration attribute. */
    void stageUsbSelfPowered(bool enabled);

    /** @brief Return the effective staged/persisted self-powered value. */
    bool usbSelfPowered();

    /** @brief Stage the requested USB bus current in milliamperes. */
    void stageUsbRequestedCurrent(unsigned milliamps);

    /** @brief Return the effective staged/persisted requested USB current. */
    unsigned usbRequestedCurrent();

    /**
     * @brief Send a raw MCP2221 protocol command.
     *
     * This is an advanced escape hatch. Prefer typed high-level methods when
     * an equivalent operation exists.
     *
     * @note The MCP2221 reset command does not return a protocol response.
     *       For that command the returned array remains zero-initialized.
     */
    std::array<std::uint8_t, 64> rawCommand(
        const std::uint8_t* command,
        std::size_t size);

private:
    std::shared_ptr<detail::DeviceState> state_;
};

} // namespace libeasymcp2221

#endif // LIBEASYMCP2221_CPP_DEVICE_H
