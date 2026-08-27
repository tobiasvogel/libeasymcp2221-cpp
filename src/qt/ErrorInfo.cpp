#include <libeasymcp2221++/qt/ErrorInfo.h>

namespace libeasymcp2221::qt {

ErrorInfo toErrorInfo(const libeasymcp2221::Error& error)
{
    return {
        error.code(),
        error.nativeCode(),
        QString::fromUtf8(error.what())
    };
}

}  // namespace libeasymcp2221::qt