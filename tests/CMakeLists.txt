add_executable(libeasymcp2221_cpp_tests
    test_public_api.cpp
)

target_compile_features(libeasymcp2221_cpp_tests
    PRIVATE
        cxx_std_17
)

target_link_libraries(libeasymcp2221_cpp_tests
    PRIVATE
        libeasymcp2221::cpp
)

add_test(
    NAME libeasymcp2221_cpp_tests
    COMMAND libeasymcp2221_cpp_tests
)
