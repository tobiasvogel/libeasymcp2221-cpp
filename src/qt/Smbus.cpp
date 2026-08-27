#include <libeasymcp2221++/qt/Smbus.h>

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
            "SMBus byte count must not be negative");
    }

    return static_cast<std::size_t>(size);
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

std::vector<std::uint8_t> toVector(
    const QByteArray& data)
{
    if (data.isEmpty()) {
        return {};
    }

    const auto* begin =
        reinterpret_cast<const std::uint8_t*>(
            data.constData());

    return std::vector<std::uint8_t>(
        begin,
        begin + data.size());
}

}  // namespace

QByteArray readBlockData(
    SmbusDevice& device,
    std::uint8_t command)
{
    return toQByteArray(
        device.readBlockData(command));
}

void writeBlockData(
    SmbusDevice& device,
    std::uint8_t command,
    const QByteArray& data)
{
    device.writeBlockData(
        command,
        toVector(data));
}

QByteArray blockProcessCall(
    SmbusDevice& device,
    std::uint8_t command,
    const QByteArray& data)
{
    return toQByteArray(
        device.blockProcessCall(
            command,
            toVector(data)));
}

QByteArray readI2cBlockData(
    SmbusDevice& device,
    std::uint8_t command,
    qsizetype size)
{
    return toQByteArray(
        device.readI2cBlockData(
            command,
            checkedSize(size)));
}

void writeI2cBlockData(
    SmbusDevice& device,
    std::uint8_t command,
    const QByteArray& data)
{
    device.writeI2cBlockData(
        command,
        toVector(data));
}

}  // namespace libeasymcp2221::qt