cmake_minimum_required(VERSION 3.10)
project(libeasymcp2221_cpp VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(LIBEASYMCP2221_CPP_BUILD_SHARED "Build shared C++ library" ON)
option(LIBEASYMCP2221_CPP_BUILD_STATIC "Build static C++ library" ON)
option(LIBEASYMCP2221_CPP_BUILD_EXAMPLES "Build examples" ON)
option(LIBEASYMCP2221_CPP_BUILD_TESTS "Build unit tests" OFF)
option(LIBEASYMCP2221_CPP_BUILD_DOCS "Build API documentation with Doxygen" OFF)
option(
    LIBEASYMCP2221_CPP_DOXYGEN_AWESOME
    "Use Doxygen Awesome for generated HTML documentation"
    ON
)

if (NOT LIBEASYMCP2221_CPP_BUILD_SHARED AND
    NOT LIBEASYMCP2221_CPP_BUILD_STATIC)
    message(FATAL_ERROR
        "At least one of LIBEASYMCP2221_CPP_BUILD_SHARED or "
        "LIBEASYMCP2221_CPP_BUILD_STATIC must be ON"
    )
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEASYMCP2221 REQUIRED IMPORTED_TARGET libeasymcp2221)

set(LIBEASYMCP2221_CPP_SOURCES
    src/Device.cpp
    src/I2cDevice.cpp
    src/SmbusDevice.cpp
    src/GpioPoller.cpp
    src/Error.cpp
    src/detail/DeviceState.cpp
)

set(LIBEASYMCP2221_CPP_INSTALL_TARGETS)
set(LIBEASYMCP2221_CPP_EXAMPLE_LINK_TARGET)

function(libeasymcp2221_cpp_configure_target target)
    target_compile_features(${target} PUBLIC cxx_std_17)

    target_include_directories(${target}
        PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE
            ${PROJECT_SOURCE_DIR}/src
    )

    target_link_libraries(${target}
        PRIVATE
            PkgConfig::LIBEASYMCP2221
    )

    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME "easymcp2221++"
    )
endfunction()

if (LIBEASYMCP2221_CPP_BUILD_SHARED)
    add_library(easymcp2221_cpp SHARED ${LIBEASYMCP2221_CPP_SOURCES})
    libeasymcp2221_cpp_configure_target(easymcp2221_cpp)

    set_target_properties(easymcp2221_cpp PROPERTIES
        EXPORT_NAME cpp
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )

    add_library(libeasymcp2221::cpp ALIAS easymcp2221_cpp)

    list(APPEND
        LIBEASYMCP2221_CPP_INSTALL_TARGETS
        easymcp2221_cpp
    )
    set(LIBEASYMCP2221_CPP_EXAMPLE_LINK_TARGET easymcp2221_cpp)
endif()

if (LIBEASYMCP2221_CPP_BUILD_STATIC)
    add_library(
        easymcp2221_cpp_static
        STATIC
        ${LIBEASYMCP2221_CPP_SOURCES}
    )
    libeasymcp2221_cpp_configure_target(easymcp2221_cpp_static)

    if (LIBEASYMCP2221_CPP_BUILD_SHARED)
        set_target_properties(easymcp2221_cpp_static PROPERTIES
            EXPORT_NAME cpp_static
        )
    else()
        set_target_properties(easymcp2221_cpp_static PROPERTIES
            EXPORT_NAME cpp
        )
    endif()

    add_library(
        libeasymcp2221::cpp_static
        ALIAS
        easymcp2221_cpp_static
    )

    list(APPEND
        LIBEASYMCP2221_CPP_INSTALL_TARGETS
        easymcp2221_cpp_static
    )

    if (NOT LIBEASYMCP2221_CPP_EXAMPLE_LINK_TARGET)
        set(
            LIBEASYMCP2221_CPP_EXAMPLE_LINK_TARGET
            easymcp2221_cpp_static
        )
    endif()
endif()

install(TARGETS ${LIBEASYMCP2221_CPP_INSTALL_TARGETS}
    EXPORT libeasymcp2221-cpp-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

set(LIBEASYMCP2221_CPP_CMAKE_INSTALL_DIR
    "${CMAKE_INSTALL_LIBDIR}/cmake/libeasymcp2221-cpp"
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/libeasymcp2221-cppConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/libeasymcp2221-cppConfig.cmake
    INSTALL_DESTINATION ${LIBEASYMCP2221_CPP_CMAKE_INSTALL_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/libeasymcp2221-cppConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(EXPORT libeasymcp2221-cpp-targets
    FILE libeasymcp2221-cppTargets.cmake
    NAMESPACE libeasymcp2221::
    DESTINATION ${LIBEASYMCP2221_CPP_CMAKE_INSTALL_DIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libeasymcp2221-cppConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/libeasymcp2221-cppConfigVersion.cmake
    DESTINATION ${LIBEASYMCP2221_CPP_CMAKE_INSTALL_DIR}
)

# pkg-config metadata mirrors the C library's installation model.
set(
    LIBEASYMCP2221_CPP_PKGCONFIG_LIBDIR
    "${CMAKE_INSTALL_FULL_LIBDIR}"
)
set(
    LIBEASYMCP2221_CPP_PKGCONFIG_INCLUDEDIR
    "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/libeasymcp2221++.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/libeasymcp2221++.pc
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libeasymcp2221++.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
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

        set(LIBEASYMCP2221_CPP_DOXYGEN_AWESOME_STYLESHEETS
            "${DOXYGEN_AWESOME_CSS_DIR}/doxygen-awesome.css "
            "${DOXYGEN_AWESOME_CSS_DIR}/doxygen-awesome-sidebar-only.css"
        )

        # CMake lists are semicolon-separated. Doxygen expects
        # HTML_EXTRA_STYLESHEET entries to be whitespace-separated, so join
        # the list explicitly before substituting it into Doxyfile.
        string(
            JOIN " "
            LIBEASYMCP2221_CPP_DOXYGEN_EXTRA_STYLESHEETS
            ${LIBEASYMCP2221_CPP_DOXYGEN_AWESOME_STYLESHEETS}
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
