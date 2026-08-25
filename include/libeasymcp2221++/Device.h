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
     * @return true when this object still references shared device state.
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

    /**
     * @brief Create a stateful adapter for one SMBus target.
     * @param address 7-bit SMBus target address.
     * @return Stateful SMBus target adapter.
     */
    SmbusDevice smbusDevice(std::uint8_t address);

    /**
     * @brief Create a move-only stateful GPIO poller.
     * @return Stateful GPIO poller sharing this device context.
     */
    GpioPoller gpioPoller();

    /**
     * @brief Set the MCP2221 I2C bus clock frequency.
     * @param hz Requested I2C clock frequency in hertz.
     */
    void setI2cSpeed(std::uint32_t hz);

    /**
     * @brief Perform a raw I2C write using the default transfer watchdog.
     * @param address 7-bit I2C target address.
     * @param data Payload pointer.
     * @param size Number of payload bytes.
     * @param transfer Transfer mode.
     */
    void i2cWrite(
        std::uint8_t address,
        const std::uint8_t* data,
        std::size_t size,
        I2cTransfer transfer = I2cTransfer::Normal);

    /**
     * @brief Convenience overload for std::vector payloads.
     * @param address 7-bit I2C target address.
     * @param data Payload bytes.
     * @param transfer Transfer mode.
     */
    void i2cWrite(
        std::uint8_t address,
        const std::vector<std::uint8_t>& data,
        I2cTransfer transfer = I2cTransfer::Normal);

    /**
     * @brief Perform a raw I2C read using the default transfer watchdog.
     * @param address 7-bit I2C target address.
     * @param size Number of bytes to read.
     * @param transfer Transfer mode.
     * @return Received payload bytes.
     */
    std::vector<std::uint8_t> i2cRead(
        std::uint8_t address,
        std::size_t size,
        I2cTransfer transfer = I2cTransfer::Normal);

    /**
     * @brief Return a typed I2C-engine status snapshot.
     * @return Current I2C engine status.
     */
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

    /**
     * @brief Read the three ADC channels as raw 10-bit values.
     * @return Raw ADC readings for the three channels.
     */
    std::array<std::uint16_t, 3> readAdcRaw();

    /**
     * @brief Read the three ADC channels as normalized values.
     * @return Normalized ADC readings for the three channels.
     */
    std::array<double, 3> readAdcNormalized();

    /**
     * @brief Read the three ADC channels as voltages.
     * @return ADC readings in volts for the three channels.
     */
    std::array<double, 3> readAdcVolts();

    /**
     * @brief Configure the DAC voltage reference while preserving output code.
     *
     * The underlying C helper retains the EasyMCP2221 workaround for MCP2221
     * reference-transition quirks.
     *
     * @param reference Strongly typed DAC reference selection.
     */
    void configureDac(VoltageReference reference);

    /**
     * @brief Configure the DAC reference and raw 5-bit output code.
     * @param reference Strongly typed DAC reference selection.
     * @param outputCode Raw DAC code from 0 through 31.
     */
    void configureDac(VoltageReference reference, std::uint8_t outputCode);

    /**
     * @brief Write a raw 5-bit DAC code.
     * @param code Raw DAC output code from 0 through 31.
     */
    void writeDacRaw(std::uint8_t code);

    /**
     * @brief Write a normalized DAC output value.
     * @param value Normalized DAC output value.
     */
    void writeDacNormalized(double value);

    /**
     * @brief Write a DAC output voltage.
     * @param volts Requested DAC output voltage.
     */
    void writeDacVolts(double volts);

    /**
     * @brief Configure the MCP2221 clock output.
     * @param duty Clock duty-cycle selection.
     * @param frequency Clock-output frequency selection.
     */
    void configureClock(ClockDutyCycle duty, ClockFrequency frequency);

    /**
     * @brief Read the interrupt-on-change flag.
     * @return true when the IOC flag is currently set.
     */
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

    /**
     * @brief Read one raw persistent flash section.
     * @param section Section to read.
     * @return Complete 60-byte raw section payload.
     * @throws Error on flash-read, protocol, or transport failure.
     */
    FlashData readFlash(FlashSection section);

    /**
     * @brief Write one raw persistent flash section.
     *
     * @warning This is a low-level persistent write. The caller is responsible
     *          for supplying a valid 60-byte payload for the selected section.
     *
     * @param section Section to write.
     * @param data Complete 60-byte raw payload.
     * @throws Error on flash-write, protocol, or transport failure.
     */
    void writeFlash(FlashSection section, const FlashData& data);

    /**
     * @brief Send the eight-byte flash access password.
     * @param password Password bytes.
     * @throws Error if the device rejects the password or transport fails.
     */
    void sendFlashPassword(const FlashPassword& password);

    /**
     * @brief Read aggregate persistent flash information.
     *
     * USB string descriptors are decoded by the underlying C library using its
     * existing best-effort UTF-16LE-to-UTF-8 conversion.
     *
     * @return Aggregate raw and decoded persistent flash information.
     */
    FlashInfo flashInfo();

    /**
     * @brief Read raw persistent chip and GPIO flash settings.
     * @return Raw chip-settings and GP-settings snapshots.
     */
    FlashSettings flashSettings();

    /**
     * @brief Persist the current staged/runtime configuration to flash.
     *
     * @warning This operation performs persistent writes and is not atomic.
     *          Chip settings are written before GP settings; if the second
     *          write fails, chip settings may already have been persisted.
     *          Staged USB settings remain staged when the save fails.
     */
    void saveConfigurationToFlash();

    /**
     * @brief Stage the USB Remote Wake-up capability.
     *
     * This changes only the configuration cached in the device context.
     * Persist it with saveConfigurationToFlash(). The USB host observes the
     * persisted value only after device re-enumeration.
     *
     * @param enabled true to advertise Remote Wake-up capability.
     */
    void stageUsbRemoteWakeup(bool enabled);

    /**
     * @brief Return the effective USB Remote Wake-up setting.
     *
     * A staged value takes precedence over the value currently stored in flash.
     * This does not report whether the host OS has enabled wake-up for the
     * device.
     *
     * @return Effective Remote Wake-up capability setting.
     */
    bool usbRemoteWakeup();

    /**
     * @brief Stage whether the device advertises itself as self-powered.
     *
     * This changes only the USB enumeration attribute; it does not change the
     * physical power source. Persist with saveConfigurationToFlash().
     *
     * @param enabled true for self-powered, false for bus-powered.
     */
    void stageUsbSelfPowered(bool enabled);

    /**
     * @brief Return the effective self-powered enumeration setting.
     *
     * A staged value takes precedence over the value currently stored in flash.
     *
     * @return Effective self-powered USB enumeration setting.
     */
    bool usbSelfPowered();

    /**
     * @brief Stage the requested USB bus current in milliamperes.
     *
     * The value describes the device to the USB host; it does not electrically
     * limit or regulate current. Valid values are even numbers from 0 through
     * 500 mA inclusive. Persist with saveConfigurationToFlash().
     *
     * @param milliamps Requested current in mA.
     * @throws Error when the value is odd, exceeds 500 mA, or the device
     *         operation fails.
     */
    void stageUsbRequestedCurrent(unsigned milliamps);

    /**
     * @brief Return the effective requested USB bus current in milliamperes.
     *
     * A staged value takes precedence over the value currently stored in flash.
     * The returned value is decoded to mA, not a raw USBREQCRT register value.
     *
     * @return Effective requested USB current in milliamperes.
     */
    unsigned usbRequestedCurrent();

    /**
     * @brief Send a raw MCP2221 protocol command.
     *
     * This is an advanced escape hatch. Prefer typed high-level methods when
     * an equivalent operation exists.
     *
     * @param command Command bytes.
     * @param size Number of command bytes.
     * @return 64-byte MCP2221 response buffer.
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
