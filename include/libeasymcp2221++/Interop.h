/**
 * @file Interop.h
 * @brief Optional native-handle interoperation helpers.
 */

#ifndef LIBEASYMCP2221_CPP_INTEROP_H
#define LIBEASYMCP2221_CPP_INTEROP_H

namespace libeasymcp2221 {

class Device;

/**
 * @brief Marker header reserved for explicit C/C++ interoperation.
 *
 * Native-handle access is intentionally not part of the initial public API.
 * It can be introduced here later without exposing C headers through Device.h.
 */

}  // namespace libeasymcp2221

#endif	// LIBEASYMCP2221_CPP_INTEROP_H
