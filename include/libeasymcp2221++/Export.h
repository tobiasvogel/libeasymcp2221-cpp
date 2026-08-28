/**
 * @file Export.h
 * @brief Shared-library import/export annotations.
 */

#ifndef LIBEASYMCP2221_CPP_EXPORT_H
#define LIBEASYMCP2221_CPP_EXPORT_H

#if defined(_WIN32) || defined(__CYGWIN__)

#  if defined(LIBEASYMCP2221_CPP_STATIC)
#    define LIBEASYMCP2221_CPP_API
#  elif defined(LIBEASYMCP2221_CPP_BUILDING)
#    define LIBEASYMCP2221_CPP_API __declspec(dllexport)
#  else
#    define LIBEASYMCP2221_CPP_API __declspec(dllimport)
#  endif

#  if defined(LIBEASYMCP2221_CPP_QT_STATIC)
#    define LIBEASYMCP2221_CPP_QT_API
#  elif defined(LIBEASYMCP2221_CPP_QT_BUILDING)
#    define LIBEASYMCP2221_CPP_QT_API __declspec(dllexport)
#  else
#    define LIBEASYMCP2221_CPP_QT_API __declspec(dllimport)
#  endif

#else

#  define LIBEASYMCP2221_CPP_API
#  define LIBEASYMCP2221_CPP_QT_API

#endif

#endif  // LIBEASYMCP2221_CPP_EXPORT_H
