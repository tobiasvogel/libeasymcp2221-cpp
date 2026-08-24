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

Both shared and static C++ libraries are built by default. Either variant can
be disabled independently:

```sh
cmake -S . -B build \
    -DLIBEASYMCP2221_CPP_BUILD_SHARED=ON \
    -DLIBEASYMCP2221_CPP_BUILD_STATIC=ON
cmake --build build
cmake --install build
```

The installed CMake package exports:

- `libeasymcp2221::cpp` for the shared library, when built.
- `libeasymcp2221::cpp_static` for the static library, when built.

A consuming CMake project can use:

```cmake
find_package(libeasymcp2221-cpp CONFIG REQUIRED)

target_link_libraries(my_application
    PRIVATE
        libeasymcp2221::cpp
)
```

The underlying C library is discovered through its installed `libeasymcp2221`
pkg-config metadata. A `libeasymcp2221-cpp.pc` file is installed as well for
non-CMake consumers.

