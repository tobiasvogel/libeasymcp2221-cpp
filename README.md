# libeasymcp2221++

Object-oriented C++17 adapter for
[`libeasymcp2221`](https://github.com/tobiasvogel/libeasymcp2221).

This repository is initially scaffolded around four stateful public objects:

- `libeasymcp2221::Device`
- `libeasymcp2221::I2cDevice`
- `libeasymcp2221::SmbusDevice`
- `libeasymcp2221::GpioPoller`

The C library remains the sole implementation of USB transport and MCP2221
protocol logic. The C++ layer adds RAII, ownership, synchronization, strongly
typed enums/value types, exceptions, and container-oriented return values.

## Documentation

Public interfaces and implementation-facing types are documented in Doxygen
syntax from the beginning. Build documentation with:

```sh
cmake -S . -B build -DLIBEASYMCP2221_CPP_BUILD_DOCS=ON
cmake --build build --target docs
```
