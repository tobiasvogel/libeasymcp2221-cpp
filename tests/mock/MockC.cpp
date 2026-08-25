/**
 * @file MockC.cpp
 * @brief Hardware-free mock implementation of the libeasymcp2221 C API.
 */

#include "MockControl.h"

#include <array>
#include <cstring>
#include <string>

extern "C" {
#include <libeasymcp2221/mcp2221.h>
#include <libeasymcp2221/mcp2221_analog.h>
#include <libeasymcp2221/mcp2221_flash.h>
#include <libeasymcp2221/mcp2221_flash_info.h>
#include <libeasymcp2221/mcp2221_flash_settings.h>
#include <libeasymcp2221/mcp2221_gpio.h>
#include <libeasymcp2221/mcp2221_gpio_poll.h>
#include <libeasymcp2221/mcp2221_i2c_slave.h>
#include <libeasymcp2221/mcp2221_pin.h>
#include <libeasymcp2221/mcp2221_smbus.h>
#include <libeasymcp2221/mcp2221_sram.h>
#include <libeasymcp2221/mcp2221_usb.h>
}

struct mcp2221_device {
    double vdd = 3.3;
};

namespace {

struct MockState {
    mcp2221_error_code_t nextError = MCP2221_ERR_OK;
    int opens = 0;
    int closes = 0;
    int usbCurrentSetCalls = 0;
    unsigned usbCurrent = 100;
    int remoteWakeup = 0;
    int selfPowered = 0;
    int clockDuty = -1;
    std::string clockFrequency;
    std::array<int, 4> gpioWrite{{-1, -1, -1, -1}};
    std::uint16_t gpioFilterMask = 0;
};

MockState state;

mcp2221_error_code_t consumeError()
{
    const auto error = state.nextError;
    state.nextError = MCP2221_ERR_OK;
    return error;
}

} // namespace

namespace libeasymcp2221_test {

void resetMock() { state = MockState{}; }
void failNext(mcp2221_error_code_t error) { state.nextError = error; }
int openCount() { return state.opens; }
int closeCount() { return state.closes; }
int usbCurrentSetCount() { return state.usbCurrentSetCalls; }
int lastClockDuty() { return state.clockDuty; }
std::string lastClockFrequency() { return state.clockFrequency; }
int lastGpio0() { return state.gpioWrite[0]; }
int lastGpio1() { return state.gpioWrite[1]; }
int lastGpio2() { return state.gpioWrite[2]; }
int lastGpio3() { return state.gpioWrite[3]; }
std::uint16_t lastGpioFilterMask() { return state.gpioFilterMask; }

} // namespace libeasymcp2221_test

extern "C" {

const char* mcp2221_error_code_to_string(mcp2221_error_code_t code)
{
    switch (code) {
    case MCP2221_ERR_OK: return "MCP2221_ERR_OK";
    case MCP2221_ERR_TIMEOUT: return "MCP2221_ERR_TIMEOUT";
    case MCP2221_ERR_NOT_ACK: return "MCP2221_ERR_NOT_ACK";
    case MCP2221_ERR_INVALID: return "MCP2221_ERR_INVALID";
    case MCP2221_ERR_PROTOCOL: return "MCP2221_ERR_PROTOCOL";
    default: return "MCP2221_ERR_MOCK";
    }
}

mcp2221_error_code_t mcp2221_open_scan(
    uint16_t, uint16_t, int, const char*, int, int, int, int, int,
    mcp2221_t** out_dev)
{
    const auto error = consumeError();
    if (error != MCP2221_ERR_OK) {
        if (out_dev != nullptr) {
            *out_dev = nullptr;
        }
        return error;
    }
    if (out_dev == nullptr) {
        return MCP2221_ERR_INVALID;
    }
    *out_dev = new mcp2221_device{};
    ++state.opens;
    return MCP2221_ERR_OK;
}

void mcp2221_close(mcp2221_t* dev)
{
    if (dev != nullptr) {
        ++state.closes;
        delete dev;
    }
}

mcp2221_error_code_t mcp2221_send_cmd(
    mcp2221_t*, const uint8_t*, size_t, uint8_t* response)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && response != nullptr) {
        std::memset(response, 0, 64);
    }
    return error;
}

mcp2221_error_code_t mcp2221_i2c_set_speed(mcp2221_t*, uint32_t)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_i2c_write_simple(
    mcp2221_t*, uint8_t, const uint8_t*, size_t, mcp2221_i2c_kind_t)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_i2c_read_simple(
    mcp2221_t*, uint8_t, uint8_t* data, size_t len, mcp2221_i2c_kind_t)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && data != nullptr) {
        std::memset(data, 0x5A, len);
    }
    return error;
}

mcp2221_error_code_t mcp2221_i2c_status(
    mcp2221_t*, mcp2221_i2c_status_t* status)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && status != nullptr) {
        std::memset(status, 0, sizeof(*status));
        status->scl = 1;
        status->sda = 1;
        status->initialized = 1;
    }
    return error;
}

mcp2221_error_code_t mcp2221_i2c_release(mcp2221_t*)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_i2c_slave_init(
    mcp2221_i2c_slave_t* slave, mcp2221_t* mcp, uint8_t addr, int,
    uint32_t, int reg_bytes, mcp2221_i2c_byte_order_t byte_order)
{
    const auto error = consumeError();
    if (error != MCP2221_ERR_OK) {
        return error;
    }
    slave->mcp = mcp;
    slave->addr = addr;
    slave->reg_bytes = reg_bytes;
    slave->reg_byteorder = byte_order;
    return MCP2221_ERR_OK;
}

mcp2221_error_code_t mcp2221_i2c_slave_check_present(
    mcp2221_i2c_slave_t*, int* is_present)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && is_present != nullptr) {
        *is_present = 1;
    }
    return error;
}

mcp2221_error_code_t mcp2221_i2c_slave_read(
    mcp2221_i2c_slave_t*, uint8_t* buffer, size_t length)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && buffer != nullptr) {
        std::memset(buffer, 0xA5, length);
    }
    return error;
}

mcp2221_error_code_t mcp2221_i2c_slave_write(
    mcp2221_i2c_slave_t*, const uint8_t*, size_t)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_i2c_slave_read_register(
    mcp2221_i2c_slave_t*, uint32_t, uint8_t* buffer, size_t length,
    int, mcp2221_i2c_byte_order_t)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && buffer != nullptr) {
        std::memset(buffer, 0x3C, length);
    }
    return error;
}

mcp2221_error_code_t mcp2221_i2c_slave_write_register(
    mcp2221_i2c_slave_t*, uint32_t, const uint8_t*, size_t,
    int, mcp2221_i2c_byte_order_t)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_smbus_init(
    mcp2221_smbus_t* bus, mcp2221_t* existing_mcp, int, uint16_t, uint16_t,
    const char*, uint32_t)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK) {
        bus->mcp = existing_mcp;
        bus->owns_mcp = 0;
    }
    return error;
}

void mcp2221_smbus_close(mcp2221_smbus_t* bus)
{
    if (bus != nullptr) {
        bus->mcp = nullptr;
        bus->owns_mcp = 0;
    }
}

mcp2221_error_code_t mcp2221_smbus_read_byte(
    mcp2221_smbus_t*, uint8_t, uint8_t* value)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && value != nullptr) *value = 0x12;
    return error;
}

mcp2221_error_code_t mcp2221_smbus_write_byte(
    mcp2221_smbus_t*, uint8_t, uint8_t)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_smbus_read_byte_data(
    mcp2221_smbus_t*, uint8_t, uint8_t, uint8_t* value)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && value != nullptr) *value = 0x34;
    return error;
}

mcp2221_error_code_t mcp2221_smbus_write_byte_data(
    mcp2221_smbus_t*, uint8_t, uint8_t, uint8_t)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_smbus_read_word_data(
    mcp2221_smbus_t*, uint8_t, uint8_t, int16_t* value)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && value != nullptr) *value = 0x1234;
    return error;
}

mcp2221_error_code_t mcp2221_smbus_write_word_data(
    mcp2221_smbus_t*, uint8_t, uint8_t, int16_t)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_smbus_process_call(
    mcp2221_smbus_t*, uint8_t, uint8_t, int16_t value, int16_t* response)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && response != nullptr) *response = value;
    return error;
}

mcp2221_error_code_t mcp2221_smbus_read_block_data(
    mcp2221_smbus_t*, uint8_t, uint8_t, uint8_t* buffer, size_t* length)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && buffer != nullptr && length != nullptr) {
        buffer[0] = 1;
        buffer[1] = 2;
        *length = 2;
    }
    return error;
}

mcp2221_error_code_t mcp2221_smbus_write_block_data(
    mcp2221_smbus_t*, uint8_t, uint8_t, const uint8_t*, size_t)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_smbus_block_process_call(
    mcp2221_smbus_t*, uint8_t, uint8_t, const uint8_t*, size_t,
    uint8_t* response, size_t* response_length)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && response != nullptr &&
        response_length != nullptr) {
        response[0] = 0x55;
        *response_length = 1;
    }
    return error;
}

mcp2221_error_code_t mcp2221_smbus_read_i2c_block_data(
    mcp2221_smbus_t*, uint8_t, uint8_t, uint8_t* buffer, size_t length)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && buffer != nullptr) {
        std::memset(buffer, 0x66, length);
    }
    return error;
}

mcp2221_error_code_t mcp2221_smbus_write_i2c_block_data(
    mcp2221_smbus_t*, uint8_t, uint8_t, const uint8_t*, size_t)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_gpio_read_mask(
    mcp2221_t*, int gpio_state[4], uint8_t* valid_mask)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK) {
        gpio_state[0] = 0;
        gpio_state[1] = 1;
        gpio_state[2] = -1;
        gpio_state[3] = -1;
        *valid_mask = 0x03;
    }
    return error;
}

mcp2221_error_code_t mcp2221_gpio_write(
    mcp2221_t*, const mcp2221_gpio_write_t* values)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && values != nullptr) {
        state.gpioWrite = {{values->gp0, values->gp1, values->gp2, values->gp3}};
    }
    return error;
}

mcp2221_error_code_t mcp2221_pin_set_function(
    mcp2221_t*, mcp2221_gpio_pin_t, mcp2221_pin_function_t)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_pin_set_functions(
    mcp2221_t*, const mcp2221_pin_functions_t*)
{
    return consumeError();
}

void mcp2221_gpio_poll_init(mcp2221_gpio_poll_state_t* poll)
{
    if (poll != nullptr) std::memset(poll, 0, sizeof(*poll));
}

void mcp2221_gpio_poll_set_filter_mask(
    mcp2221_gpio_poll_state_t* poll, uint16_t mask)
{
    state.gpioFilterMask = mask;
    if (poll != nullptr) poll->filter_mask = mask;
}

mcp2221_error_code_t mcp2221_gpio_poll(
    mcp2221_t*, mcp2221_gpio_poll_state_t*, mcp2221_gpio_change_t changes[4])
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && changes != nullptr) {
        std::memset(changes, 0, sizeof(mcp2221_gpio_change_t) * 4);
    }
    return error;
}

int mcp2221_gpio_poll_events(
    mcp2221_t*, mcp2221_gpio_poll_state_t*, const uint16_t*,
    mcp2221_gpio_event_t*, size_t)
{
    const auto error = consumeError();
    return error == MCP2221_ERR_OK ? 0 : static_cast<int>(error);
}

mcp2221_error_code_t mcp2221_sram_config(
    mcp2221_t*, const mcp2221_sram_config_t*)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_analog_set_vdd(mcp2221_t* dev, double volts)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK) dev->vdd = volts;
    return error;
}

mcp2221_error_code_t mcp2221_analog_get_vdd(
    const mcp2221_t* dev, double* volts)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && volts != nullptr) *volts = dev->vdd;
    return error;
}

mcp2221_error_code_t mcp2221_adc_config(mcp2221_t*, const char*)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_adc_read_raw(mcp2221_t*, uint16_t values[3])
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK) {
        values[0] = 1; values[1] = 2; values[2] = 3;
    }
    return error;
}

mcp2221_error_code_t mcp2221_adc_read_normalized(mcp2221_t*, double values[3])
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK) {
        values[0] = 0.1; values[1] = 0.2; values[2] = 0.3;
    }
    return error;
}

mcp2221_error_code_t mcp2221_adc_read_volts(mcp2221_t*, double values[3])
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK) {
        values[0] = 1.0; values[1] = 2.0; values[2] = 3.0;
    }
    return error;
}

mcp2221_error_code_t mcp2221_dac_config(mcp2221_t*, const char*)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_dac_config_out(mcp2221_t*, const char*, int)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_dac_write_raw(mcp2221_t*, uint8_t)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_dac_write_normalized(mcp2221_t*, double)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_dac_write_volts(mcp2221_t*, double)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_clock_config(
    mcp2221_t*, int duty_percent, const char* frequency)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK) {
        state.clockDuty = duty_percent;
        state.clockFrequency = frequency != nullptr ? frequency : "";
    }
    return error;
}

mcp2221_error_code_t mcp2221_ioc_read(mcp2221_t*, uint8_t* flag)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && flag != nullptr) *flag = 0;
    return error;
}

mcp2221_error_code_t mcp2221_ioc_clear(mcp2221_t*)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_ioc_config(mcp2221_t*, const char*)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_flash_read(
    mcp2221_t*, uint8_t, uint8_t data[60])
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && data != nullptr) std::memset(data, 0, 60);
    return error;
}

mcp2221_error_code_t mcp2221_flash_write(
    mcp2221_t*, uint8_t, const uint8_t[60])
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_flash_send_password(
    mcp2221_t*, const uint8_t[8])
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_flash_read_info(
    mcp2221_t*, mcp2221_flash_info_t* info)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && info != nullptr) {
        std::memset(info, 0, sizeof(*info));
    }
    return error;
}

mcp2221_error_code_t mcp2221_flash_get_settings(
    mcp2221_t*, mcp2221_flash_settings_t* settings)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && settings != nullptr) {
        std::memset(settings, 0, sizeof(*settings));
    }
    return error;
}

mcp2221_error_code_t mcp2221_flash_save_config(mcp2221_t*)
{
    return consumeError();
}

mcp2221_error_code_t mcp2221_usb_set_remote_wakeup(mcp2221_t*, int enabled)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK) state.remoteWakeup = enabled != 0;
    return error;
}

mcp2221_error_code_t mcp2221_usb_get_remote_wakeup(
    mcp2221_t*, int* enabled)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && enabled != nullptr) *enabled = state.remoteWakeup;
    return error;
}

mcp2221_error_code_t mcp2221_usb_set_self_powered(mcp2221_t*, int enabled)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK) state.selfPowered = enabled != 0;
    return error;
}

mcp2221_error_code_t mcp2221_usb_get_self_powered(
    mcp2221_t*, int* enabled)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && enabled != nullptr) *enabled = state.selfPowered;
    return error;
}

mcp2221_error_code_t mcp2221_usb_set_requested_current(
    mcp2221_t*, unsigned ma)
{
    ++state.usbCurrentSetCalls;
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK) state.usbCurrent = ma;
    return error;
}

mcp2221_error_code_t mcp2221_usb_get_requested_current(
    mcp2221_t*, unsigned* ma)
{
    const auto error = consumeError();
    if (error == MCP2221_ERR_OK && ma != nullptr) *ma = state.usbCurrent;
    return error;
}

} // extern "C"
