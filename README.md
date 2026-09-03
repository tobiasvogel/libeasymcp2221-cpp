[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
# libeasymcp2221++

Object-oriented C++17 adapter for
[`libeasymcp2221`](https://github.com/tobiasvogel/libeasymcp2221).

The public API is centered around four stateful objects:

- `libeasymcp2221::Device`
- `libeasymcp2221::I2cDevice`
- `libeasymcp2221::SmbusDevice`
- `libeasymcp2221::GpioPoller`

The C library remains the sole implementation of USB transport and MCP2221
protocol logic. The C++ layer adds RAII, ownership, synchronization, strongly
typed enums/value types, exceptions, and container-oriented return values.

`libeasymcp2221++` is therefore an adapter rather than a second MCP2221
protocol implementation: it keeps the tested C core while providing an
idiomatic C++17 interface on top of it.

## Documentation

Public interfaces and implementation-facing types are documented in Doxygen
syntax. Build documentation with:

```sh
cmake -S . -B build -DLIBEASYMCP2221_CPP_BUILD_DOCS=ON
cmake --build build --target docs
```

[Doxygen Awesome](https://github.com/jothepro/doxygen-awesome-css) is enabled
by default for documentation builds and pinned to release `v2.4.2`. Disable it
with `-DLIBEASYMCP2221_CPP_DOXYGEN_AWESOME=OFF` when plain Doxygen HTML is
preferred or when configuring without network access.

## Build and install

The C++ adapter requires `libeasymcp2221` 2.0.1 or newer.

Both shared and static C++ libraries are built by default. Examples are
disabled by default and can be enabled with
`-DLIBEASYMCP2221_CPP_BUILD_EXAMPLES=ON`. Either library variant can be
disabled independently:

```sh
cmake -S . -B build \
    -DLIBEASYMCP2221_CPP_BUILD_SHARED=ON \
    -DLIBEASYMCP2221_CPP_BUILD_STATIC=ON
cmake --build build
cmake --install build
```

The installed CMake package exports:

`libeasymcp2221::cpp` is the primary target. It refers to the shared library
when a shared build is available, and to the static library in a static-only
build. When shared and static variants are installed together, the static
variant is additionally available as `libeasymcp2221::cpp_static`.

A consuming CMake project can use:

```cmake
find_package(libeasymcp2221-cpp CONFIG REQUIRED)

target_link_libraries(my_application
    PRIVATE
        libeasymcp2221::cpp
)
```

The underlying C library is discovered through its installed `libeasymcp2221`
pkg-config metadata. A `libeasymcp2221++.pc` file is installed as well for
non-CMake consumers.

### Optional Qt 6 integration

Enable the optional Qt integration when configuring the project:

```sh
cmake -S . -B build \
    -DLIBEASYMCP2221_CPP_BUILD_QT=ON
```

The Qt integration requires Qt 6 Core and remains separate from the Qt-free
core library. Installed CMake consumers request it as a package component:

```cmake
find_package(
    libeasymcp2221-cpp
    CONFIG
    REQUIRED
    COMPONENTS Qt
)

target_link_libraries(my_application
    PRIVATE
        libeasymcp2221::qt
)
```

When shared and static variants are installed together,
`libeasymcp2221::qt` refers to the shared Qt integration library and
`libeasymcp2221::qt_static` is also available. In a static-only installation,
`libeasymcp2221::qt` refers to the static Qt integration library.

## Tests

The unit tests use a hardware-free mock implementation of the
`libeasymcp2221` C API. An installed `libeasymcp2221` development package is
still required for the public C headers, but no MCP2221 device needs to be
connected.

```sh
cmake -S . -B build -DLIBEASYMCP2221_CPP_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

The hardware-free suite covers error translation, shared device lifetime,
raw and stateful I2C, SMBus, GPIO sentinel mapping, analog/clock value
translation, and USB-side argument validation.

## Public include path

The C++ headers are installed below `include/libeasymcp2221++` so they can
coexist with the C library headers below `include/libeasymcp2221`.

For the complete public C++ API:

```cpp
#include <libeasymcp2221++/libeasymcp2221++.h>
```

The C++ namespace remains `libeasymcp2221`.


## Versioning and releases

The project follows semantic versioning. During the `0.x` series, compatibility
is guaranteed within a minor release line; for example, `0.1.x` releases are
intended to remain source-compatible with `0.1.0`.

Release notes are maintained in [`CHANGELOG.md`](CHANGELOG.md). GitHub releases
are source releases; users are expected to build against an installed
`libeasymcp2221` dependency appropriate for their platform.

## Author
Tobias X. Vogel

## License

`libeasymcp2221++` is distributed under the MIT License. See
[`LICENSE`](LICENSE).
