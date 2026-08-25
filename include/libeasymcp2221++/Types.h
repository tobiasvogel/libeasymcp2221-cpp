/**
 * @file Types.h
 * @brief Strongly typed value types used by the public C++ API.
 */

#ifndef LIBEASYMCP2221_CPP_TYPES_H
#define LIBEASYMCP2221_CPP_TYPES_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

namespace libeasymcp2221 {

/** @brief MCP2221 general-purpose pin identifiers. */
enum class Pin : std::uint8_t {
    GP0 = 0,
    GP1 = 1,
    GP2 = 2,
    GP3 = 3
};

/** @brief High-level MCP2221 pin functions. */
enum class PinFunction {
    Dedicated,
    Alt0,
    Alt1,
    Alt2,
    GpioInput,
    GpioOutput
};

/** @brief GPIO direction selector used by SRAM configuration. */
enum class GpioDirection {
    Output,
    Input
};

/**
 * @brief SRAM-level GP function selector.
 *
 * SRAM stores the GPIO function selector independently from GPIO direction,
 * therefore this type deliberately does not encode input/output direction.
 */
enum class SramPinFunction {
    Gpio,
    Dedicated,
    Alt0,
    Alt1,
    Alt2
};

/** @brief I2C transfer sequencing mode. */
enum class I2cTransfer {
    Normal,
    RepeatedStart,
    NoStop
};

/** @brief Register-address byte order used by I2C target helpers. */
enum class ByteOrder {
    BigEndian,
    LittleEndian
};

/** @brief MCP2221 analog voltage-reference selection. */
enum class VoltageReference {
    Off,
    Vdd,
    Internal1V024,
    Internal2V048,
    Internal4V096
};

/** @brief Interrupt-on-change edge selection. */
enum class InterruptEdge {
    None,
    Rising,
    Falling,
    Both
};

/** @brief GPIO edge-event type. */
enum class GpioEdge {
    Rising,
    Falling
};

/** @brief Supported clock-output frequencies. */
enum class ClockFrequency {
    KHz375,
    KHz750,
    MHz1_5,
    MHz3,
    MHz6,
    MHz12,
    MHz24
};

/** @brief Supported clock-output duty cycles. */
enum class ClockDutyCycle {
    Percent0,
    Percent25,
    Percent50,
    Percent75
};

/** @brief Public persistent flash section identifiers. */
enum class FlashSection {
    ChipSettings,
    GpSettings,
    UsbManufacturer,
    UsbProduct,
    UsbSerial,
    ChipSerial
};

/** @brief One complete raw MCP2221 flash-section payload. */
using FlashData = std::array<std::uint8_t, 60>;

/** @brief MCP2221 flash access password. */
using FlashPassword = std::array<std::uint8_t, 8>;

/**
 * @brief Snapshot of the MCP2221 I2C engine state.
 *
 * This is the typed C++ counterpart of the C API's mcp2221_i2c_status_t.
 */
struct I2cStatus {
    std::uint16_t requestedLength = 0;
    std::uint16_t transmittedLength = 0;
    std::uint8_t divider = 0;
    bool acknowledged = false;
    std::uint8_t state = 0;
    bool scl = false;
    bool sda = false;
    bool confused = false;
    bool initialized = false;
};

/**
 * @brief Current logical state of the four GP pins.
 *
 * A disengaged std::optional means that the corresponding pin is not
 * configured as GPIO.
 */
struct GpioState {
    std::array<std::optional<bool>, 4> pins{};
};

/**
 * @brief Partial GPIO output update.
 *
 * A disengaged std::optional preserves the corresponding output.
 */
struct GpioWrite {
    std::array<std::optional<bool>, 4> pins{};
};

/** @brief State transition information for one GPIO pin. */
struct GpioChange {
    std::optional<bool> oldValue;
    std::optional<bool> newValue;
    bool changed = false;
};

/** @brief One GPIO edge event produced by a GpioPoller. */
struct GpioEvent {
    Pin pin = Pin::GP0;
    GpioEdge edge = GpioEdge::Rising;
    double time = 0.0;
    double previousTime = 0.0;

    /**
     * @brief Return a compatibility identifier such as "GPIO0_RISE".
     * @return Event identifier derived from @ref pin and @ref edge.
     */
    std::string id() const;
};

/**
 * @brief Partial configuration of one GP pin.
 *
 * A disengaged @ref function leaves the pin configuration unchanged.
 */
struct PinConfiguration {
    std::optional<PinFunction> function;
    bool outputValue = false;
};

/** @brief Batch configuration for GP0 through GP3. */
using PinConfigurations = std::array<PinConfiguration, 4>;

/**
 * @brief Partial SRAM configuration for one GP pin.
 *
 * Every disengaged field preserves the current device value.
 */
struct SramPinConfig {
    /** @brief Optional GPIO output latch value. */
    std::optional<bool> outputValue;

    /** @brief Optional GPIO direction. */
    std::optional<GpioDirection> direction;

    /** @brief Optional SRAM pin-function selector. */
    std::optional<SramPinFunction> function;
};

/**
 * @brief Partial SRAM interrupt-on-change configuration.
 *
 * Every disengaged field preserves the current device value.
 */
struct SramInterruptConfig {
    std::optional<bool> risingEdge;
    std::optional<bool> fallingEdge;
    std::optional<bool> clearFlag;
};

/**
 * @brief Partial SRAM ADC reference configuration.
 *
 * The value maps to the pair of native VRM/reference-source fields.
 */
struct SramAdcConfig {
    std::optional<VoltageReference> reference;
};

/**
 * @brief Partial SRAM DAC configuration.
 *
 * The reference maps to the native VRM/reference-source fields. @ref value is
 * the raw 5-bit DAC output code from 0 through 31.
 */
struct SramDacConfig {
    std::optional<VoltageReference> reference;
    std::optional<std::uint8_t> value;
};

/**
 * @brief Partial SRAM clock-output configuration.
 *
 * Frequency and duty cycle are independently optional so either field can be
 * changed without disturbing the other.
 */
struct SramClockConfig {
    std::optional<ClockFrequency> frequency;
    std::optional<ClockDutyCycle> dutyCycle;
};

/**
 * @brief Aggregated runtime SRAM configuration.
 *
 * A default-constructed object changes nothing. Every disengaged
 * std::optional preserves the corresponding current SRAM field.
 *
 * @note Applying this type changes runtime SRAM only. It does not write flash.
 */
struct SramConfig {
    std::array<SramPinConfig, 4> gpio{};
    SramInterruptConfig interrupt{};
    SramAdcConfig adc{};
    SramDacConfig dac{};
    SramClockConfig clock{};
};

/** @brief Raw persistent chip and GP flash settings. */
struct FlashSettings {
    FlashData chip{};
    FlashData gpio{};
};

/** @brief Aggregate persistent flash information. */
struct FlashInfo {
    FlashData chipSettings{};
    FlashData gpSettings{};
    FlashData usbManufacturerRaw{};
    FlashData usbProductRaw{};
    FlashData usbSerialRaw{};
    FlashData factorySerialRaw{};

    std::string usbManufacturer;
    std::string usbProduct;
    std::string usbSerial;
    std::string factorySerial;
};

} // namespace libeasymcp2221

#endif // LIBEASYMCP2221_CPP_TYPES_H
