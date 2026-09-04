cmake_minimum_required(VERSION 3.11)
project(libeasymcp2221_cpp VERSION 0.2.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(LIBEASYMCP2221_CPP_BUILD_SHARED "Build shared C++ library" ON)
option(LIBEASYMCP2221_CPP_BUILD_STATIC "Build static C++ library" ON)
option(LIBEASYMCP2221_CPP_BUILD_QT "Build optional Qt integration library" OFF)
option(LIBEASYMCP2221_CPP_BUILD_EXAMPLES "Build examples" OFF)
option(LIBEASYMCP2221_CPP_BUILD_TESTS "Build unit tests" OFF)
option(LIBEASYMCP2221_CPP_BUILD_DOCS "Build API documentation with Doxygen" OFF)
option(
    LIBEASYMCP2221_CPP_DOXYGEN_AWESOME
    "Use Doxygen Awesome for generated HTML documentation"
    ON
)
set(
    LIBEASYMCP2221_CPP_DOXYGEN_AWESOME_PATH
    ""
    CACHE PATH
    "Path to an installed Doxygen Awesome CSS directory; avoids FetchContent"
)

if (NOT LIBEASYMCP2221_CPP_BUILD_SHARED AND
    NOT LIBEASYMCP2221_CPP_BUILD_STATIC)
    message(FATAL_ERROR
        "At least one of LIBEASYMCP2221_CPP_BUILD_SHARED or "
        "LIBEASYMCP2221_CPP_BUILD_STATIC must be ON"
    )
endif()

if (MSVC)
    find_path(
        LIBEASYMCP2221_INCLUDE_DIR
        NAMES libeasymcp2221/mcp2221.h
    )
    if (NOT LIBEASYMCP2221_INCLUDE_DIR)
        message(FATAL_ERROR
            "libeasymcp2221 headers were not found for MSVC. Build and "
            "install libeasymcp2221 with MSVC first and make its install "
            "prefix available through CMAKE_PREFIX_PATH."
        )
    endif()

    if (LIBEASYMCP2221_CPP_BUILD_SHARED)
        find_library(
            LIBEASYMCP2221_SHARED_LIBRARY
            NAMES libeasymcp2221
        )
        if (NOT LIBEASYMCP2221_SHARED_LIBRARY)
            message(FATAL_ERROR
                "The libeasymcp2221 MSVC shared import library was not "
                "found. Build and install the shared C library first."
            )
        endif()

        add_library(libeasymcp2221_cpp_c_shared UNKNOWN IMPORTED)
        set_target_properties(
            libeasymcp2221_cpp_c_shared
            PROPERTIES
                IMPORTED_LOCATION "${LIBEASYMCP2221_SHARED_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${LIBEASYMCP2221_INCLUDE_DIR}"
        )
        set(
            LIBEASYMCP2221_CPP_C_SHARED_TARGET
            libeasymcp2221_cpp_c_shared
        )
    endif()

    if (LIBEASYMCP2221_CPP_BUILD_STATIC)
        find_library(
            LIBEASYMCP2221_STATIC_LIBRARY
            NAMES libeasymcp2221_static
        )
        if (NOT LIBEASYMCP2221_STATIC_LIBRARY)
            message(FATAL_ERROR
                "The libeasymcp2221 MSVC static library was not found. "
                "Build and install the static C library first."
            )
        endif()

        find_library(
            LIBEASYMCP2221_LIBUSB_LIBRARY
            NAMES libusb-1.0
        )
        if (NOT LIBEASYMCP2221_LIBUSB_LIBRARY)
            message(FATAL_ERROR
                "libusb-1.0 was not found for the static libeasymcp2221 "
                "dependency."
            )
        endif()

        add_library(libeasymcp2221_cpp_c_static UNKNOWN IMPORTED)
        set_target_properties(
            libeasymcp2221_cpp_c_static
            PROPERTIES
                IMPORTED_LOCATION "${LIBEASYMCP2221_STATIC_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${LIBEASYMCP2221_INCLUDE_DIR}"
                INTERFACE_COMPILE_DEFINITIONS LIBEASYMCP2221_STATIC
                INTERFACE_LINK_LIBRARIES "${LIBEASYMCP2221_LIBUSB_LIBRARY}"
        )
        set(
            LIBEASYMCP2221_CPP_C_STATIC_TARGET
            libeasymcp2221_cpp_c_static
        )
    endif()

    set(LIBEASYMCP2221_INCLUDE_DIRS "${LIBEASYMCP2221_INCLUDE_DIR}")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(
        LIBEASYMCP2221
        REQUIRED
        IMPORTED_TARGET
        "libeasymcp2221>=2.0.2"
    )
    set(LIBEASYMCP2221_CPP_C_SHARED_TARGET PkgConfig::LIBEASYMCP2221)

    if (LIBEASYMCP2221_CPP_BUILD_STATIC)
        set(
            _LIBEASYMCP2221_CPP_SAVED_LIBRARY_SUFFIXES
            "${CMAKE_FIND_LIBRARY_SUFFIXES}"
        )
        set(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_STATIC_LIBRARY_SUFFIX}")
        find_library(
            LIBEASYMCP2221_STATIC_LIBRARY
            NAMES easymcp2221
            HINTS ${LIBEASYMCP2221_STATIC_LIBRARY_DIRS}
            NO_DEFAULT_PATH
        )
        set(
            CMAKE_FIND_LIBRARY_SUFFIXES
            "${_LIBEASYMCP2221_CPP_SAVED_LIBRARY_SUFFIXES}"
        )
        unset(_LIBEASYMCP2221_CPP_SAVED_LIBRARY_SUFFIXES)
        if (NOT LIBEASYMCP2221_STATIC_LIBRARY)
            message(FATAL_ERROR
                "The static libeasymcp2221 library was not found in the "
                "pkg-config library directories."
            )
        endif()

        pkg_check_modules(
            LIBEASYMCP2221_LIBUSB
            REQUIRED
            IMPORTED_TARGET
            "libusb-1.0"
        )
        find_package(Threads REQUIRED)

        set(
            LIBEASYMCP2221_CPP_STATIC_C_DEPENDENCIES
            PkgConfig::LIBEASYMCP2221_LIBUSB
            Threads::Threads
        )

        list(FIND LIBEASYMCP2221_STATIC_LIBRARIES "m"
            LIBEASYMCP2221_CPP_STATIC_MATH_INDEX)
        if (NOT LIBEASYMCP2221_CPP_STATIC_MATH_INDEX EQUAL -1)
            find_library(LIBEASYMCP2221_MATH_LIBRARY m)
            if (NOT LIBEASYMCP2221_MATH_LIBRARY)
                message(FATAL_ERROR
                    "libeasymcp2221 requires libm for static linking, but "
                    "libm was not found."
                )
            endif()
            list(APPEND
                LIBEASYMCP2221_CPP_STATIC_C_DEPENDENCIES
                "${LIBEASYMCP2221_MATH_LIBRARY}"
            )
        endif()

        add_library(libeasymcp2221_cpp_c_static STATIC IMPORTED)
        set_target_properties(
            libeasymcp2221_cpp_c_static
            PROPERTIES
                IMPORTED_LOCATION "${LIBEASYMCP2221_STATIC_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${LIBEASYMCP2221_INCLUDE_DIRS}"
                INTERFACE_COMPILE_DEFINITIONS LIBEASYMCP2221_STATIC
                INTERFACE_LINK_LIBRARIES
                    "${LIBEASYMCP2221_CPP_STATIC_C_DEPENDENCIES}"
        )
        set(
            LIBEASYMCP2221_CPP_C_STATIC_TARGET
            libeasymcp2221_cpp_c_static
        )
    endif()
endif()

if (LIBEASYMCP2221_CPP_BUILD_QT)
    if (CMAKE_VERSION VERSION_LESS 3.16)
        message(FATAL_ERROR
            "The Qt integration requires CMake 3.16 or newer"
        )
    endif()

    find_package(Qt6 REQUIRED COMPONENTS Core)
endif()

set(LIBEASYMCP2221_CPP_SOURCES
    src/Device.cpp
    src/I2cDevice.cpp
    src/SmbusDevice.cpp
    src/GpioPoller.cpp
    src/Error.cpp
    src/detail/DeviceState.cpp
)

set(LIBEASYMCP2221_CPP_QT_SOURCES
    src/qt/ErrorInfo.cpp
    src/qt/GpioMonitor.cpp
    src/qt/I2c.cpp
    src/qt/Smbus.cpp
    include/libeasymcp2221++/qt/GpioMonitor.h
)

set(LIBEASYMCP2221_CPP_INSTALL_TARGETS)
set(LIBEASYMCP2221_CPP_QT_INSTALL_TARGETS)
set(LIBEASYMCP2221_CPP_EXAMPLE_LINK_TARGET)

set(
    LIBEASYMCP2221_CPP_HAS_QT
    ${LIBEASYMCP2221_CPP_BUILD_QT}
)

function(libeasymcp2221_cpp_configure_target target c_dependency_target)
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
            ${c_dependency_target}
    )

    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME "easymcp2221++"
    )
endfunction()

function(libeasymcp2221_cpp_configure_qt_target target)
    target_compile_features(${target} PUBLIC cxx_std_17)

    target_include_directories(${target}
        PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    target_link_libraries(${target}
        PUBLIC
            Qt6::Core
    )

    set_target_properties(${target} PROPERTIES
        AUTOMOC ON
        OUTPUT_NAME "easymcp2221++-qt"
    )
endfunction()

if (LIBEASYMCP2221_CPP_BUILD_SHARED)
    add_library(easymcp2221_cpp SHARED ${LIBEASYMCP2221_CPP_SOURCES})
    libeasymcp2221_cpp_configure_target(
        easymcp2221_cpp
        ${LIBEASYMCP2221_CPP_C_SHARED_TARGET}
    )

    target_compile_definitions(
        easymcp2221_cpp
        PRIVATE
            LIBEASYMCP2221_CPP_BUILDING
    )

    target_link_libraries(easymcp2221_cpp
        INTERFACE
            "$<LINK_ONLY:${LIBEASYMCP2221_CPP_C_SHARED_TARGET}>"
    )

    set_target_properties(easymcp2221_cpp PROPERTIES
        EXPORT_NAME cpp
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES
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
    libeasymcp2221_cpp_configure_target(
        easymcp2221_cpp_static
        ${LIBEASYMCP2221_CPP_C_STATIC_TARGET}
    )

    target_compile_definitions(
        easymcp2221_cpp_static
        PUBLIC
            LIBEASYMCP2221_CPP_STATIC
    )

    target_link_libraries(easymcp2221_cpp_static
        INTERFACE
            "$<LINK_ONLY:${LIBEASYMCP2221_CPP_C_STATIC_TARGET}>"
    )

    if (MSVC)
        set_target_properties(
            easymcp2221_cpp_static
            PROPERTIES
                OUTPUT_NAME "easymcp2221++_static"
        )
    endif()

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

    if (NOT LIBEASYMCP2221_CPP_BUILD_SHARED)
        add_library(
            libeasymcp2221::cpp
            ALIAS
            easymcp2221_cpp_static
        )
    endif()

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

if (LIBEASYMCP2221_CPP_BUILD_QT)
    if (LIBEASYMCP2221_CPP_BUILD_SHARED)
        add_library(
            easymcp2221_cpp_qt
            SHARED
            ${LIBEASYMCP2221_CPP_QT_SOURCES}
        )

        libeasymcp2221_cpp_configure_qt_target(
            easymcp2221_cpp_qt
        )

        target_compile_definitions(
            easymcp2221_cpp_qt
            PRIVATE
                LIBEASYMCP2221_CPP_QT_BUILDING
        )

        target_link_libraries(
            easymcp2221_cpp_qt
            PUBLIC
                easymcp2221_cpp
        )

        set_target_properties(
            easymcp2221_cpp_qt
            PROPERTIES
                EXPORT_NAME qt
                VERSION ${PROJECT_VERSION}
                SOVERSION ${PROJECT_VERSION_MAJOR}
                CXX_VISIBILITY_PRESET hidden
                VISIBILITY_INLINES_HIDDEN YES
        )

        add_library(
            libeasymcp2221::qt
            ALIAS
            easymcp2221_cpp_qt
        )

        list(APPEND
            LIBEASYMCP2221_CPP_QT_INSTALL_TARGETS
            easymcp2221_cpp_qt
        )
    endif()

    if (LIBEASYMCP2221_CPP_BUILD_STATIC)
        add_library(
            easymcp2221_cpp_qt_static
            STATIC
            ${LIBEASYMCP2221_CPP_QT_SOURCES}
        )

        libeasymcp2221_cpp_configure_qt_target(
            easymcp2221_cpp_qt_static
        )

        target_compile_definitions(
            easymcp2221_cpp_qt_static
            PUBLIC
                LIBEASYMCP2221_CPP_QT_STATIC
        )

        target_link_libraries(
            easymcp2221_cpp_qt_static
            PUBLIC
                easymcp2221_cpp_static
        )

        if (MSVC)
            set_target_properties(
                easymcp2221_cpp_qt_static
                PROPERTIES
                    OUTPUT_NAME "easymcp2221++-qt_static"
            )
        endif()

        if (LIBEASYMCP2221_CPP_BUILD_SHARED)
            set_target_properties(
                easymcp2221_cpp_qt_static
                PROPERTIES
                    EXPORT_NAME qt_static
            )
        else()
            set_target_properties(
                easymcp2221_cpp_qt_static
                PROPERTIES
                    EXPORT_NAME qt
            )
        endif()

        add_library(
            libeasymcp2221::qt_static
            ALIAS
            easymcp2221_cpp_qt_static
        )

        if (NOT LIBEASYMCP2221_CPP_BUILD_SHARED)
            add_library(
                libeasymcp2221::qt
                ALIAS
                easymcp2221_cpp_qt_static
            )
        endif()

        list(APPEND
            LIBEASYMCP2221_CPP_QT_INSTALL_TARGETS
            easymcp2221_cpp_qt_static
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

if (LIBEASYMCP2221_CPP_QT_INSTALL_TARGETS)
    install(
        TARGETS ${LIBEASYMCP2221_CPP_QT_INSTALL_TARGETS}
        EXPORT libeasymcp2221-cpp-qt-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "qt" EXCLUDE
)

if (LIBEASYMCP2221_CPP_BUILD_QT)
    install(
        DIRECTORY include/libeasymcp2221++/qt/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libeasymcp2221++/qt
        FILES_MATCHING PATTERN "*.h"
    )
endif()

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
    COMPATIBILITY SameMinorVersion
)

install(EXPORT libeasymcp2221-cpp-targets
    FILE libeasymcp2221-cppTargets.cmake
    NAMESPACE libeasymcp2221::
    DESTINATION ${LIBEASYMCP2221_CPP_CMAKE_INSTALL_DIR}
)

if (LIBEASYMCP2221_CPP_QT_INSTALL_TARGETS)
    install(EXPORT libeasymcp2221-cpp-qt-targets
        FILE libeasymcp2221-cppQtTargets.cmake
        NAMESPACE libeasymcp2221::
        DESTINATION ${LIBEASYMCP2221_CPP_CMAKE_INSTALL_DIR}
    )
endif()

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
        if (LIBEASYMCP2221_CPP_DOXYGEN_AWESOME_PATH)
            set(
                DOXYGEN_AWESOME_CSS_DIR
                "${LIBEASYMCP2221_CPP_DOXYGEN_AWESOME_PATH}"
            )

            foreach(_stylesheet
                doxygen-awesome.css
                doxygen-awesome-sidebar-only.css
            )
                if (NOT EXISTS "${DOXYGEN_AWESOME_CSS_DIR}/${_stylesheet}")
                    message(FATAL_ERROR
                        "Doxygen Awesome stylesheet not found: "
                        "${DOXYGEN_AWESOME_CSS_DIR}/${_stylesheet}"
                    )
                endif()
            endforeach()
        else()
            if (CMAKE_VERSION VERSION_LESS 3.14)
                message(FATAL_ERROR
                    "Doxygen Awesome documentation requires CMake 3.14 or newer "
                    "when it is downloaded with FetchContent. Set "
                    "LIBEASYMCP2221_CPP_DOXYGEN_AWESOME_PATH to use a local "
                    "installation with older CMake."
                )
            endif()

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
        endif()

        set(LIBEASYMCP2221_CPP_DOXYGEN_AWESOME_STYLESHEETS
            "${DOXYGEN_AWESOME_CSS_DIR}/doxygen-awesome.css"
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
