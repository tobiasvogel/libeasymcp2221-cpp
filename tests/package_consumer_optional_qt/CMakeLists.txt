cmake_minimum_required(VERSION 3.16)
project(libeasymcp2221_cpp_optional_qt_consumer LANGUAGES CXX)

find_package(
    libeasymcp2221-cpp
    CONFIG
    REQUIRED
    OPTIONAL_COMPONENTS Qt
)

if (libeasymcp2221-cpp_Qt_FOUND OR TARGET libeasymcp2221::qt)
    message(FATAL_ERROR "Optional Qt component was unexpectedly found")
endif()

add_executable(
    package_consumer
    "${CMAKE_CURRENT_SOURCE_DIR}/../package_consumer/main.cpp"
)
target_compile_features(package_consumer PRIVATE cxx_std_17)
target_link_libraries(
    package_consumer
    PRIVATE libeasymcp2221::cpp
)
