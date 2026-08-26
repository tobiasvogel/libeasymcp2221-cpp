# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project follows [Semantic Versioning](https://semver.org/).

## [Unreleased]

## [0.1.0] - 2026-08-26

### Added

- Initial public C++17 API for `libeasymcp2221` 2.x.
- RAII-based `Device` ownership with explicit `close()` support.
- Stateful `I2cDevice`, `SmbusDevice`, and `GpioPoller` child adapters.
- Raw I2C access and typed I2C register helpers.
- SMBus byte, word, block, process-call, and I2C-block helpers.
- Strongly typed GPIO, pin-function, SRAM, ADC, DAC, clock, interrupt,
  flash, and USB configuration APIs.
- C++ value types for runtime and persistent configuration.
- Exception-based error translation while preserving native C error codes.
- Hardware-free mock-backed unit tests.
- CMake package exports and pkg-config metadata for external consumers.
- Shared-only, static-only, and combined shared/static build support.
- Doxygen documentation with optional Doxygen Awesome styling.
- Compile-checked examples for I2C, SMBus, GPIO, analog, SRAM, flash, and USB.

[Unreleased]: https://github.com/tobiasvogel/libeasymcp2221-cpp/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/tobiasvogel/libeasymcp2221-cpp/releases/tag/v0.1.0
