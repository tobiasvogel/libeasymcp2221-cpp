#include <libeasymcp2221++/Device.h>

#ifdef LIBEASYMCP2221_CPP_CONSUMER_QT
#include <libeasymcp2221++/I2cDevice.h>
#include <libeasymcp2221++/qt/I2c.h>

#include <QByteArray>
#endif

int main()
{
#ifdef LIBEASYMCP2221_CPP_CONSUMER_QT
    using ReadFunction = QByteArray (*)(
        libeasymcp2221::I2cDevice&,
        qsizetype);
    const auto readFunction =
        static_cast<ReadFunction>(&libeasymcp2221::qt::read);
    return readFunction != nullptr ? 0 : 1;
#else
    const auto isOpenFunction =
        &libeasymcp2221::Device::isOpen;
    return isOpenFunction != nullptr ? 0 : 1;
#endif
}
