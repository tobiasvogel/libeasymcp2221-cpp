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
