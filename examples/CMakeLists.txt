function(libeasymcp2221_cpp_add_example name)
    add_executable(
        "libeasymcp2221_cpp_${name}"
        "${name}.cpp"
    )

    target_compile_features(
        "libeasymcp2221_cpp_${name}"
        PRIVATE
            cxx_std_17
    )

    target_link_libraries(
        "libeasymcp2221_cpp_${name}"
        PRIVATE
            ${LIBEASYMCP2221_CPP_EXAMPLE_LINK_TARGET}
    )
endfunction()

libeasymcp2221_cpp_add_example(basic)
libeasymcp2221_cpp_add_example(i2c_device)
libeasymcp2221_cpp_add_example(smbus_device)
libeasymcp2221_cpp_add_example(gpio)
libeasymcp2221_cpp_add_example(analog)
libeasymcp2221_cpp_add_example(sram)
libeasymcp2221_cpp_add_example(flash_usb)

# Parallel counterparts of libeasymcp2221 v2.0.2 examples.
libeasymcp2221_cpp_add_example(scan_i2c)
libeasymcp2221_cpp_add_example(gpio_basic)
libeasymcp2221_cpp_add_example(set_pin_functions)
libeasymcp2221_cpp_add_example(analog_demo)
libeasymcp2221_cpp_add_example(usb_power)

libeasymcp2221_cpp_add_example(gpio_poll_events)
libeasymcp2221_cpp_add_example(test_i2c)
libeasymcp2221_cpp_add_example(flash_info_save)
libeasymcp2221_cpp_add_example(ssd1306_i2c)
