# Examples

The examples are intentionally small and are compiled when
`LIBEASYMCP2221_CPP_BUILD_EXAMPLES=ON`.

```sh
cmake -S . -B build -DLIBEASYMCP2221_CPP_BUILD_EXAMPLES=ON
cmake --build build
```

They are compile-checked by the CI linkage matrix, but CI does not execute
them because they require a real MCP2221(A) and, for most examples, external
hardware connected to it.

| Example | Purpose | Hardware side effects |
| --- | --- | --- |
| `basic.cpp` | Open an MCP2221(A) | Opens/closes the USB device |
| `i2c_device.cpp` | Stateful I2C target access | Performs I2C transfers |
| `smbus_device.cpp` | Stateful SMBus target access | Performs SMBus transfers |
| `gpio.cpp` | GPIO configuration/read/write | Changes GP0/GP1 runtime state |
| `analog.cpp` | ADC reads and DAC output | Changes the physical DAC output |
| `sram.cpp` | Partial runtime SRAM configuration | Changes runtime configuration only |
| `flash_usb.cpp` | Flash info and USB staging | Stages USB settings; persistent save is commented out |
| `scan_i2c.cpp` | C++ counterpart of C `scan_i2c.c` | Probes all 7-bit I2C addresses |
| `gpio_basic.cpp` | C++ counterpart of C `gpio_basic.c` | Reconfigures and drives GP0-GP3 |
| `set_pin_functions.cpp` | C++ counterpart of C `set_pin_functions.c` | Changes GP0-GP3 runtime functions |
| `analog_demo.cpp` | C++ counterpart of C `analog_demo.c` | Changes ADC/DAC/clock runtime configuration |
| `usb_power.cpp` | C++ counterpart of C `usb_power.c` | **Persists USB power attributes to flash** |
| `gpio_poll_events.cpp` | C++ counterpart of C `gpio_poll_events.c` | Polls GP0 until a rising edge occurs |
| `test_i2c.cpp` | C++ counterpart of C `test_i2c.c` | Reads 16 bytes from a 16-bit-addressed EEPROM at `0x50` |
| `flash_info_save.cpp` | C++ counterpart of C `flash_info_save.c` | **Reads flash info and persists current configuration** |
| `ssd1306_i2c.cpp` | C++ counterpart of C `ssd1306_i2c.c` | Initializes and writes to an SSD1306 OLED at `0x3C` |
| `qt_gpio_monitor.cpp` | Qt GPIO edge monitor using `GpioMonitor` signals | Configures GP0-GP3 as GPIO inputs at runtime |
| `qt_pin_configurator.cpp` | Interactive Qt runtime pin configuration | Changes GP0-GP3 runtime functions and GPIO output states |
| `qt_analog_lab.cpp` | Live Qt ADC display and DAC control | Configures GP1/GP2 as ADC inputs and GP3 as DAC output |

## Parallel C/C++ examples

The examples whose names mirror the `libeasymcp2221` C repository intentionally
perform the same task as their C counterparts. This makes it possible to compare
explicit C handles/error codes/cleanup with the C++17 RAII, exception, and
strong-type API while keeping the hardware operation itself equivalent.

## Persistent configuration

`flash_usb.cpp` deliberately leaves `saveConfigurationToFlash()` commented
out. Enabling it performs persistent flash writes. The save operation is not
atomic: chip settings may already have been written if a later GP-settings
write fails.

`usb_power.cpp`, unlike `flash_usb.cpp`, intentionally calls
`saveConfigurationToFlash()` because it mirrors the C `usb_power.c` example.
`flash_info_save.cpp` likewise persists the current configuration because
the corresponding C example does so. Running either example therefore
performs persistent flash writes[^1].

## Qt GPIO monitor

`qt_gpio_monitor.cpp` demonstrates the Qt event-loop integration provided by
`libeasymcp2221::qt::GpioMonitor`. It counts rising and falling GPIO edges and
updates the UI directly from Qt signals without an application-managed polling
loop.

The example configures GP0 through GP3 as GPIO inputs when it starts. This
changes runtime configuration only; it does not write the configuration to
flash. The previous runtime pin configuration is not restored automatically
when the example exits[^1].

## Qt pin configurator

`qt_pin_configurator.cpp` demonstrates how the strongly typed C++ pin
configuration API can be driven from a small Qt user interface. Each GP pin
can be assigned a function and GPIO outputs can be initialized high or low.

Applying a configuration changes the current runtime pin state immediately.
The example does not write configuration to flash and does not restore the
previous runtime configuration when it exits. Selecting GPIO output may
therefore immediately change the electrical level of the corresponding pin[^1].

## Qt analog lab

`qt_analog_lab.cpp` demonstrates periodic ADC acquisition and interactive DAC
control in a small Qt application. GP1 and GP2 are configured as ADC1 and ADC2,
while GP3 is configured as the DAC2 output. GP0 is left unchanged.

The ADC values are displayed as raw 10-bit readings and the DAC slider controls
the raw 5-bit output value. Using raw values keeps the example independent of
the actual MCP2221 supply voltage.

The example changes runtime pin, ADC, DAC, and physical DAC output state. It
does not write configuration to flash and does not restore the previous runtime
configuration when it exits[^1].

[^1]: USB enumeration changes become visible to the host only after the device is
re-enumerated.

## Addresses and registers

The I2C and SMBus addresses and register numbers in these examples are
placeholders unless the example intentionally mirrors fixed hardware from the
C repository. `test_i2c.cpp` assumes an EEPROM at `0x50`; `ssd1306_i2c.cpp`
assumes a 128x32 SSD1306 at `0x3C`.
