/**
 * @file Smbus.h
 * @brief Qt convenience functions for SMBus block transfers.
 */

#ifndef LIBEASYMCP2221_CPP_QT_SMBUS_H
#define LIBEASYMCP2221_CPP_QT_SMBUS_H

#include <cstdint>

#include <QByteArray>
#include <QtGlobal>

#include <libeasymcp2221++/SmbusDevice.h>

namespace libeasymcp2221::qt {

[[nodiscard]]
QByteArray readBlockData(
    SmbusDevice& device,
    std::uint8_t command);

void writeBlockData(
    SmbusDevice& device,
    std::uint8_t command,
    const QByteArray& data);

[[nodiscard]]
QByteArray blockProcessCall(
    SmbusDevice& device,
    std::uint8_t command,
    const QByteArray& data);

[[nodiscard]]
QByteArray readI2cBlockData(
    SmbusDevice& device,
    std::uint8_t command,
    qsizetype size);

void writeI2cBlockData(
    SmbusDevice& device,
    std::uint8_t command,
    const QByteArray& data);

}  // namespace libeasymcp2221::qt

#endif  // LIBEASYMCP2221_CPP_QT_SMBUS_H