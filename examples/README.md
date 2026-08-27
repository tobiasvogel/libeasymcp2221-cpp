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
Running it therefore performs persistent flash writes.

USB enumeration changes become visible to the host only after the device is
re-enumerated.

## Addresses and registers

The I2C and SMBus addresses and register numbers in these examples are
placeholders. Replace them with values appropriate for your peripheral before
running the programs.
