cmake_minimum_required(VERSION 3.16)
project(libeasymcp2221_cpp_package_consumer LANGUAGES CXX)

option(
    LIBEASYMCP2221_CPP_CONSUMER_QT
    "Test the installed Qt component"
    OFF
)

if (LIBEASYMCP2221_CPP_CONSUMER_QT)
    find_package(
        libeasymcp2221-cpp
        CONFIG
        REQUIRED
        COMPONENTS Qt
    )
else()
    find_package(libeasymcp2221-cpp CONFIG REQUIRED)
endif()

add_executable(package_consumer main.cpp)
target_compile_features(package_consumer PRIVATE cxx_std_17)

if (LIBEASYMCP2221_CPP_CONSUMER_QT)
    target_compile_definitions(
        package_consumer
        PRIVATE LIBEASYMCP2221_CPP_CONSUMER_QT=1
    )
    target_link_libraries(
        package_consumer
        PRIVATE libeasymcp2221::qt
    )
else()
    target_link_libraries(
        package_consumer
        PRIVATE libeasymcp2221::cpp
    )
endif()
