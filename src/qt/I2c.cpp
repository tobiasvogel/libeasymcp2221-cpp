#include <libeasymcp2221++/qt/I2c.h>

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace libeasymcp2221::qt {

namespace {

std::size_t checkedSize(qsizetype size)
{
    if (size < 0) {
        throw std::invalid_argument(
            "I2C byte count must not be negative");
    }

    return static_cast<std::size_t>(size);
}

const std::uint8_t* byteData(
    const QByteArray& data) noexcept
{
    if (data.isEmpty()) {
        return nullptr;
    }

    return reinterpret_cast<const std::uint8_t*>(
        data.constData());
}

QByteArray toQByteArray(
    const std::vector<std::uint8_t>& data)
{
    if (data.empty()) {
        return {};
    }

    return QByteArray(
        reinterpret_cast<const char*>(data.data()),
        static_cast<qsizetype>(data.size()));
}

}  // namespace

QByteArray read(
    I2cDevice& device,
    qsizetype size)
{
    return toQByteArray(
        device.read(checkedSize(size)));
}

void write(
    I2cDevice& device,
    const QByteArray& data)
{
    device.write(
        byteData(data),
        static_cast<std::size_t>(data.size()));
}

QByteArray readRegister(
    I2cDevice& device,
    std::uint32_t reg,
    qsizetype size)
{
    return toQByteArray(
        device.readRegister(
            reg,
            checkedSize(size)));
}

QByteArray readRegister(
    I2cDevice& device,
    std::uint32_t reg,
    qsizetype size,
    RegisterWidth registerWidth,
    ByteOrder byteOrder)
{
    return toQByteArray(
        device.readRegister(
            reg,
            checkedSize(size),
            registerWidth,
            byteOrder));
}

void writeRegister(
    I2cDevice& device,
    std::uint32_t reg,
    const QByteArray& data)
{
    device.writeRegister(
        reg,
        byteData(data),
        static_cast<std::size_t>(data.size()));
}

void writeRegister(
    I2cDevice& device,
    std::uint32_t reg,
    const QByteArray& data,
    RegisterWidth registerWidth,
    ByteOrder byteOrder)
{
    device.writeRegister(
        reg,
        byteData(data),
        static_cast<std::size_t>(data.size()),
        registerWidth,
        byteOrder);
}

}  // namespace libeasymcp2221::qt