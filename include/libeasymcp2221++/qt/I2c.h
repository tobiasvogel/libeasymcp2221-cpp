/**
 * @file I2c.h
 * @brief Qt convenience functions for I2C target adapters.
 */

#ifndef LIBEASYMCP2221_CPP_QT_I2C_H
#define LIBEASYMCP2221_CPP_QT_I2C_H

#include <libeasymcp2221++/Export.h>

#include <cstdint>

#include <QByteArray>
#include <QtGlobal>

#include <libeasymcp2221++/I2cDevice.h>

namespace libeasymcp2221::qt {
/**
 * @addtogroup qt
 * @{
 */

/**
 * @brief Read bytes from an I2C target as a QByteArray.
 *
 * @param device I2C target adapter.
 * @param size Number of bytes to read.
 * @return Received bytes.
 * @throws libeasymcp2221::Error on transfer failure.
 * @throws std::invalid_argument if @p size is negative.
 */
[[nodiscard]]
LIBEASYMCP2221_CPP_QT_API QByteArray read(I2cDevice& device, qsizetype size);

/**
 * @brief Write a QByteArray directly to an I2C target.
 *
 * @param device I2C target adapter.
 * @param data Payload bytes.
 * @throws libeasymcp2221::Error on transfer failure.
 */
LIBEASYMCP2221_CPP_QT_API void write(I2cDevice& device, const QByteArray& data);

/**
 * @brief Read bytes from an I2C register as a QByteArray.
 *
 * Uses the register width and byte order configured on @p device.
 *
 * @param device I2C target adapter.
 * @param reg Register address.
 * @param size Number of bytes to read.
 * @return Received register payload bytes.
 */
[[nodiscard]]
LIBEASYMCP2221_CPP_QT_API QByteArray readRegister(
    I2cDevice& device,
    std::uint32_t reg,
    qsizetype size);

/**
 * @brief Read bytes from an I2C register using an explicit layout.
 *
 * @param device I2C target adapter.
 * @param reg Register address.
 * @param size Number of bytes to read.
 * @param registerWidth Register-address width.
 * @param byteOrder Register-address byte order.
 * @return Received register payload bytes.
 */
[[nodiscard]]
LIBEASYMCP2221_CPP_QT_API QByteArray readRegister(
    I2cDevice& device,
    std::uint32_t reg,
    qsizetype size,
    RegisterWidth registerWidth,
    ByteOrder byteOrder);

/**
 * @brief Write a QByteArray to an I2C register.
 *
 * Uses the register width and byte order configured on @p device.
 *
 * @param device I2C target adapter.
 * @param reg Register address.
 * @param data Payload bytes.
 */
LIBEASYMCP2221_CPP_QT_API void writeRegister(
    I2cDevice& device,
    std::uint32_t reg,
    const QByteArray& data);

/**
 * @brief Write a QByteArray to an I2C register using an explicit layout.
 *
 * @param device I2C target adapter.
 * @param reg Register address.
 * @param data Payload bytes.
 * @param registerWidth Register-address width.
 * @param byteOrder Register-address byte order.
 */
LIBEASYMCP2221_CPP_QT_API void writeRegister(
    I2cDevice& device,
    std::uint32_t reg,
    const QByteArray& data,
    RegisterWidth registerWidth,
    ByteOrder byteOrder);

/** @} */

}  // namespace libeasymcp2221::qt

#endif  // LIBEASYMCP2221_CPP_QT_I2C_H