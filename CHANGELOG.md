# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project follows [Semantic Versioning](https://semver.org/).

## [Unreleased]

## [0.2.0] - 2026-08-29

### Added

- Optional Qt 6 Core integration in namespace `libeasymcp2221::qt`.
- Qt-friendly `QByteArray` wrappers for I2C and SMBus operations.
- `ErrorInfo` for carrying core errors through Qt signal boundaries.
- `GpioMonitor`, a `QObject`-based GPIO edge monitor driven by `QTimer`.
- Qt metatype declarations for GPIO and error value types.
- Dedicated Debian runtime and development packages for the Qt component.
- Debian ABI symbol tracking for the Qt shared library.
- Native Windows support for both MSVC and MinGW-w64.
- Explicit Windows DLL import/export annotations for the core and Qt APIs.
- Native MSVC discovery of the underlying `libeasymcp2221` dependency without
  requiring pkg-config.
- Windows CI for MSVC and MinGW-w64, including shared/static core and Qt
  installed-package consumers.
- Binary release packaging for Debian and Windows SDK archives.

### Changed

- Package version advanced to 0.2.0.
- Installed CMake package handling now reports a missing Qt component cleanly
  when the Qt development package is not installed.
- Installed-package consumer tests now cover core and Qt package exports.
- Static/shared Windows library naming avoids import-library collisions on MSVC.
- Qt exception tests explicitly consume intentional `[[nodiscard]]` results.

### Fixed

- Corrected `GpioPoller` move operations for its incomplete PImpl type.
- Corrected `GpioMonitor` timer ownership by using a parent-owned `QTimer`.
- Aligned empty I2C write semantics between the core and Qt wrappers.
- Corrected Windows test definitions so mock C symbols are not treated as
  DLL imports.
- Corrected MinGW/MSYS2 path handling for `PKG_CONFIG_PATH` and install prefixes.
- Corrected Debian Qt symbols template formatting and RTTI handling.

## [0.1.0] - 2026-08-27

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
- GitHub Pages documentation deployment from the Debian documentation package.
- Debian runtime, development, and documentation packages with multi-architecture
  snapshot builds, Lintian checks, hardening, and ABI symbol tracking.
- Compile-checked API examples for I2C, SMBus, GPIO, analog, SRAM, flash, and USB.
- Parallel C/C++ examples mirroring the `libeasymcp2221` v2.0.2 examples for
  direct side-by-side comparison.
- C-to-C++ API comparison and migration guide covering ownership, errors, I2C,
  SMBus, GPIO, polling, analog features, SRAM, flash, and USB configuration.

[Unreleased]: https://github.com/tobiasvogel/libeasymcp2221-cpp/compare/v0.2.0...HEAD
[0.2.0]: https://github.com/tobiasvogel/libeasymcp2221-cpp/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/tobiasvogel/libeasymcp2221-cpp/releases/tag/v0.1.0
