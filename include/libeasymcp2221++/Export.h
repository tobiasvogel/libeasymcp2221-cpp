/**
 * @file Export.h
 * @brief Shared-library import/export annotations.
 */

#ifndef LIBEASYMCP2221_CPP_EXPORT_H
#define LIBEASYMCP2221_CPP_EXPORT_H

#if defined(_WIN32) || defined(__CYGWIN__)

#  if defined(LIBEASYMCP2221_CPP_STATIC)
#    define LIBEASYMCP2221_CPP_API
#    define LIBEASYMCP2221_CPP_LOCAL
#  elif defined(LIBEASYMCP2221_CPP_BUILDING)
#    define LIBEASYMCP2221_CPP_API __declspec(dllexport)
#  else
#    define LIBEASYMCP2221_CPP_API __declspec(dllimport)
#  endif

#  if !defined(LIBEASYMCP2221_CPP_STATIC)
#    define LIBEASYMCP2221_CPP_LOCAL
#  endif

#  define LIBEASYMCP2221_CPP_CLASS_API

#  if defined(LIBEASYMCP2221_CPP_QT_STATIC)
#    define LIBEASYMCP2221_CPP_QT_API
#  elif defined(LIBEASYMCP2221_CPP_QT_BUILDING)
#    define LIBEASYMCP2221_CPP_QT_API __declspec(dllexport)
#  else
#    define LIBEASYMCP2221_CPP_QT_API __declspec(dllimport)
#  endif

#elif defined(__GNUC__) || defined(__clang__)

#  if defined(LIBEASYMCP2221_CPP_STATIC)
#    define LIBEASYMCP2221_CPP_API
#    define LIBEASYMCP2221_CPP_LOCAL
#  else
#    define LIBEASYMCP2221_CPP_API __attribute__((visibility("default")))
#    define LIBEASYMCP2221_CPP_LOCAL __attribute__((visibility("hidden")))
#  endif

#  define LIBEASYMCP2221_CPP_CLASS_API LIBEASYMCP2221_CPP_API

#  if defined(LIBEASYMCP2221_CPP_QT_STATIC)
#    define LIBEASYMCP2221_CPP_QT_API
#  else
#    define LIBEASYMCP2221_CPP_QT_API __attribute__((visibility("default")))
#  endif

#else

#  define LIBEASYMCP2221_CPP_API
#  define LIBEASYMCP2221_CPP_LOCAL
#  define LIBEASYMCP2221_CPP_CLASS_API LIBEASYMCP2221_CPP_API
#  define LIBEASYMCP2221_CPP_QT_API

#endif

#endif  // LIBEASYMCP2221_CPP_EXPORT_H
