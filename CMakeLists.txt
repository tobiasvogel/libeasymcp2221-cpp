cmake_minimum_required(VERSION 3.10)
project(libeasymcp2221_cpp VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(LIBEASYMCP2221_CPP_BUILD_EXAMPLES "Build examples" ON)
option(LIBEASYMCP2221_CPP_BUILD_TESTS "Build unit tests" OFF)
option(LIBEASYMCP2221_CPP_BUILD_DOCS "Build API documentation with Doxygen" OFF)
option(
    LIBEASYMCP2221_CPP_DOXYGEN_AWESOME
    "Use Doxygen Awesome for generated HTML documentation"
    ON
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEASYMCP2221 REQUIRED IMPORTED_TARGET libeasymcp2221)

add_library(easymcp2221_cpp
    src/Device.cpp
    src/I2cDevice.cpp
    src/SmbusDevice.cpp
    src/GpioPoller.cpp
    src/Error.cpp
    src/detail/DeviceState.cpp
)

add_library(libeasymcp2221::cpp ALIAS easymcp2221_cpp)

target_include_directories(easymcp2221_cpp
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(easymcp2221_cpp
    PRIVATE
        PkgConfig::LIBEASYMCP2221
)

set_target_properties(easymcp2221_cpp PROPERTIES
    OUTPUT_NAME "easymcp2221++"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(TARGETS easymcp2221_cpp
    EXPORT libeasymcp2221-cpp-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT libeasymcp2221-cpp-targets
    NAMESPACE libeasymcp2221::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libeasymcp2221-cpp
)

if (LIBEASYMCP2221_CPP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (LIBEASYMCP2221_CPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if (LIBEASYMCP2221_CPP_BUILD_DOCS)
    find_package(Doxygen REQUIRED)

    set(LIBEASYMCP2221_CPP_DOXYGEN_EXTRA_STYLESHEETS "")

    if (LIBEASYMCP2221_CPP_DOXYGEN_AWESOME)
        include(FetchContent)

        FetchContent_Declare(
            doxygen-awesome-css
            GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
            GIT_TAG v2.4.2
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(doxygen-awesome-css)

        FetchContent_GetProperties(
            doxygen-awesome-css
            SOURCE_DIR DOXYGEN_AWESOME_CSS_DIR
        )

        set(
            LIBEASYMCP2221_CPP_DOXYGEN_EXTRA_STYLESHEETS
            "${DOXYGEN_AWESOME_CSS_DIR}/doxygen-awesome.css "
            "${DOXYGEN_AWESOME_CSS_DIR}/doxygen-awesome-sidebar-only.css"
        )

    endif()

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY
    )

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating libeasymcp2221++ API documentation"
        VERBATIM
    )
endif()
