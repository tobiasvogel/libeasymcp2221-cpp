set(LIBEASYMCP2221_CPP_TEST_SOURCES)
foreach(source IN LISTS LIBEASYMCP2221_CPP_SOURCES)
    list(APPEND
        LIBEASYMCP2221_CPP_TEST_SOURCES
        "${PROJECT_SOURCE_DIR}/${source}"
    )
endforeach()

add_library(libeasymcp2221_cpp_test_mock STATIC
    mock/MockC.cpp
)

target_compile_features(libeasymcp2221_cpp_test_mock
    PUBLIC cxx_std_17
)

target_include_directories(libeasymcp2221_cpp_test_mock
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mock
    PRIVATE
        ${LIBEASYMCP2221_INCLUDE_DIRS}
)

add_library(libeasymcp2221_cpp_test_impl STATIC
    ${LIBEASYMCP2221_CPP_TEST_SOURCES}
)

target_compile_features(libeasymcp2221_cpp_test_impl
    PUBLIC cxx_std_17
)

target_include_directories(libeasymcp2221_cpp_test_impl
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${LIBEASYMCP2221_INCLUDE_DIRS}
)

target_link_libraries(libeasymcp2221_cpp_test_impl
    PRIVATE
        libeasymcp2221_cpp_test_mock
)

add_executable(libeasymcp2221_cpp_tests
    test_core.cpp
)

target_compile_features(libeasymcp2221_cpp_tests
    PRIVATE cxx_std_17
)

target_include_directories(libeasymcp2221_cpp_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/mock
        ${LIBEASYMCP2221_INCLUDE_DIRS}
)

target_link_libraries(libeasymcp2221_cpp_tests
    PRIVATE
        libeasymcp2221_cpp_test_impl
        libeasymcp2221_cpp_test_mock
)

add_test(
    NAME libeasymcp2221_cpp_tests
    COMMAND libeasymcp2221_cpp_tests
)

if (LIBEASYMCP2221_CPP_BUILD_QT)
    find_package(Qt6 REQUIRED COMPONENTS Test)

    set(LIBEASYMCP2221_CPP_QT_TEST_SOURCES)
    foreach(source IN LISTS LIBEASYMCP2221_CPP_QT_SOURCES)
        list(APPEND
            LIBEASYMCP2221_CPP_QT_TEST_SOURCES
            "${PROJECT_SOURCE_DIR}/${source}"
        )
    endforeach()

    add_library(libeasymcp2221_cpp_qt_test_impl STATIC
        ${LIBEASYMCP2221_CPP_QT_TEST_SOURCES}
    )

    target_compile_features(libeasymcp2221_cpp_qt_test_impl
        PUBLIC cxx_std_17
    )

    target_include_directories(libeasymcp2221_cpp_qt_test_impl
        PUBLIC
            ${PROJECT_SOURCE_DIR}/include
        PRIVATE
            ${PROJECT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/mock
            ${LIBEASYMCP2221_INCLUDE_DIRS}
    )

    target_link_libraries(libeasymcp2221_cpp_qt_test_impl
        PRIVATE
            Qt6::Core
            libeasymcp2221_cpp_test_impl
            libeasymcp2221_cpp_test_mock
    )

    set_target_properties(libeasymcp2221_cpp_qt_test_impl PROPERTIES
        AUTOMOC ON
    )

    add_executable(libeasymcp2221_cpp_qt_tests
        test_qt.cpp
    )

    target_compile_features(libeasymcp2221_cpp_qt_tests
        PRIVATE cxx_std_17
    )

    target_include_directories(libeasymcp2221_cpp_qt_tests
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/mock
            ${LIBEASYMCP2221_INCLUDE_DIRS}
    )

    target_link_libraries(libeasymcp2221_cpp_qt_tests
        PRIVATE
            Qt6::Core
            Qt6::Test
            libeasymcp2221_cpp_qt_test_impl
            libeasymcp2221_cpp_test_impl
            libeasymcp2221_cpp_test_mock
    )

    set_target_properties(libeasymcp2221_cpp_qt_tests PROPERTIES
        AUTOMOC ON
    )

    add_test(
        NAME libeasymcp2221_cpp_qt_tests
        COMMAND libeasymcp2221_cpp_qt_tests
    )
endif()

set(
    LIBEASYMCP2221_CPP_TEST_INSTALL_PREFIX
    "${PROJECT_BINARY_DIR}/test-install"
)

add_test(
    NAME libeasymcp2221_cpp_package_install
    COMMAND
        ${CMAKE_COMMAND}
        --install "${PROJECT_BINARY_DIR}"
        --prefix "${LIBEASYMCP2221_CPP_TEST_INSTALL_PREFIX}"
)
set_tests_properties(
    libeasymcp2221_cpp_package_install
    PROPERTIES
        FIXTURES_SETUP libeasymcp2221_cpp_package
)

set(
    LIBEASYMCP2221_CPP_CORE_CONSUMER_BUILD_DIR
    "${PROJECT_BINARY_DIR}/test-package-consumer-core"
)

add_test(
    NAME libeasymcp2221_cpp_package_core_configure
    COMMAND
        ${CMAKE_COMMAND}
        -S "${CMAKE_CURRENT_SOURCE_DIR}/package_consumer"
        -B "${LIBEASYMCP2221_CPP_CORE_CONSUMER_BUILD_DIR}"
        "-DCMAKE_PREFIX_PATH=${LIBEASYMCP2221_CPP_TEST_INSTALL_PREFIX}"
        -DLIBEASYMCP2221_CPP_CONSUMER_QT=OFF
)
set_tests_properties(
    libeasymcp2221_cpp_package_core_configure
    PROPERTIES
        FIXTURES_REQUIRED libeasymcp2221_cpp_package
)

add_test(
    NAME libeasymcp2221_cpp_package_core_build
    COMMAND
        ${CMAKE_COMMAND}
        --build "${LIBEASYMCP2221_CPP_CORE_CONSUMER_BUILD_DIR}"
)
set_tests_properties(
    libeasymcp2221_cpp_package_core_build
    PROPERTIES
        DEPENDS libeasymcp2221_cpp_package_core_configure
        FIXTURES_REQUIRED libeasymcp2221_cpp_package
)

if (LIBEASYMCP2221_CPP_BUILD_QT)
    set(
        LIBEASYMCP2221_CPP_QT_CONSUMER_BUILD_DIR
        "${PROJECT_BINARY_DIR}/test-package-consumer-qt"
    )

    add_test(
        NAME libeasymcp2221_cpp_package_qt_configure
        COMMAND
            ${CMAKE_COMMAND}
            -S "${CMAKE_CURRENT_SOURCE_DIR}/package_consumer"
            -B "${LIBEASYMCP2221_CPP_QT_CONSUMER_BUILD_DIR}"
            "-DCMAKE_PREFIX_PATH=${LIBEASYMCP2221_CPP_TEST_INSTALL_PREFIX}"
            -DLIBEASYMCP2221_CPP_CONSUMER_QT=ON
    )
    set_tests_properties(
        libeasymcp2221_cpp_package_qt_configure
        PROPERTIES
            FIXTURES_REQUIRED libeasymcp2221_cpp_package
    )

    add_test(
        NAME libeasymcp2221_cpp_package_qt_build
        COMMAND
            ${CMAKE_COMMAND}
            --build "${LIBEASYMCP2221_CPP_QT_CONSUMER_BUILD_DIR}"
    )
    set_tests_properties(
        libeasymcp2221_cpp_package_qt_build
        PROPERTIES
            DEPENDS libeasymcp2221_cpp_package_qt_configure
            FIXTURES_REQUIRED libeasymcp2221_cpp_package
    )
endif()
