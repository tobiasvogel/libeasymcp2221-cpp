add_executable(libeasymcp2221_cpp_basic basic.cpp)

target_link_libraries(libeasymcp2221_cpp_basic
    PRIVATE
        ${LIBEASYMCP2221_CPP_EXAMPLE_LINK_TARGET}
)
