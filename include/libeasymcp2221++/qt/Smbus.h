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
/**
 * @addtogroup qt
 * @{
 */

/**
 * @brief Read a length-prefixed SMBus block as a QByteArray.
 *
 * @param device SMBus target adapter.
 * @param command SMBus command byte.
 * @return Payload bytes without the SMBus length prefix.
 */
[[nodiscard]]
QByteArray readBlockData(
    SmbusDevice& device,
    std::uint8_t command);

/**
 * @brief Write a length-prefixed SMBus block from a QByteArray.
 *
 * @param device SMBus target adapter.
 * @param command SMBus command byte.
 * @param data Payload bytes.
 */
void writeBlockData(
    SmbusDevice& device,
    std::uint8_t command,
    const QByteArray& data);

/**
 * @brief Perform an SMBus block process call.
 *
 * @param device SMBus target adapter.
 * @param command SMBus command byte.
 * @param data Payload bytes to send.
 * @return Response payload bytes.
 */
[[nodiscard]]
QByteArray blockProcessCall(
    SmbusDevice& device,
    std::uint8_t command,
    const QByteArray& data);

/**
 * @brief Read a fixed-length I2C-style block through SMBus helpers.
 *
 * @param device SMBus target adapter.
 * @param command SMBus command byte.
 * @param size Number of bytes to read.
 * @return Requested payload bytes.
 */
[[nodiscard]]
QByteArray readI2cBlockData(
    SmbusDevice& device,
    std::uint8_t command,
    qsizetype size);

/**
 * @brief Write a fixed-length I2C-style block through SMBus helpers.
 *
 * @param device SMBus target adapter.
 * @param command SMBus command byte.
 * @param data Payload bytes.
 */
void writeI2cBlockData(
    SmbusDevice& device,
    std::uint8_t command,
    const QByteArray& data);

/** @} */

}  // namespace libeasymcp2221::qt

#endif  // LIBEASYMCP2221_CPP_QT_SMBUS_H