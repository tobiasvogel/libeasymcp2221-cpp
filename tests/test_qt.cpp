#include <libeasymcp2221++/Device.h>
#include <libeasymcp2221++/qt/ErrorInfo.h>
#include <libeasymcp2221++/qt/GpioMonitor.h>
#include <libeasymcp2221++/qt/I2c.h>
#include <libeasymcp2221++/qt/Smbus.h>

#include "mock/MockControl.h"

#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include <chrono>
#include <stdexcept>

extern "C" {
#include <libeasymcp2221/mcp2221_gpio_poll.h>
}

namespace {

using libeasymcp2221::Device;
using libeasymcp2221::GpioEvent;
using libeasymcp2221::GpioEventFilter;
using libeasymcp2221::Pin;
using libeasymcp2221::qt::ErrorInfo;
using libeasymcp2221::qt::GpioMonitor;

class QtIntegrationTest : public QObject {
    Q_OBJECT

  private Q_SLOTS:
    void init()
    {
        libeasymcp2221_test::resetMock();
    }

    void i2cReadReturnsQByteArray()
    {
        Device device;
        auto target = device.i2cDevice(0x48);

        QCOMPARE(
            libeasymcp2221::qt::read(target, 3),
            QByteArray(3, static_cast<char>(0xA5)));
    }

    void i2cRegisterReadReturnsQByteArray()
    {
        Device device;
        auto target = device.i2cDevice(0x48);

        QCOMPARE(
            libeasymcp2221::qt::readRegister(target, 0x10, 4),
            QByteArray(4, static_cast<char>(0x3C)));
    }

    void i2cRejectsNegativeSize()
    {
        Device device;
        auto target = device.i2cDevice(0x48);

        QVERIFY_EXCEPTION_THROWN(
            (void)libeasymcp2221::qt::read(target, -1),
            std::invalid_argument);
    }

    void smbusBlockReadReturnsQByteArray()
    {
        Device device;
        auto target = device.smbusDevice(0x5A);

        QCOMPARE(
            libeasymcp2221::qt::readBlockData(target, 0x01),
            QByteArray::fromHex("0102"));
    }

    void smbusBlockProcessCallReturnsQByteArray()
    {
        Device device;
        auto target = device.smbusDevice(0x5A);

        QCOMPARE(
            libeasymcp2221::qt::blockProcessCall(
                target,
                0x02,
                QByteArray::fromHex("1020")),
            QByteArray::fromHex("55"));
    }

    void smbusI2cBlockReadReturnsQByteArray()
    {
        Device device;
        auto target = device.smbusDevice(0x5A);

        QCOMPARE(
            libeasymcp2221::qt::readI2cBlockData(target, 0x03, 3),
            QByteArray(3, static_cast<char>(0x66)));
    }


    void i2cWriteForwardsPayload()
    {
        Device device;
        auto target = device.i2cDevice(0x48);

        libeasymcp2221::qt::write(
            target,
            QByteArray::fromHex("001122ff"));

        const auto& data =
            libeasymcp2221_test::lastI2cWriteData();
        QCOMPARE(
            QByteArray(
                reinterpret_cast<const char*>(data.data()),
                static_cast<qsizetype>(data.size())),
            QByteArray::fromHex("001122ff"));
    }

    void i2cRegisterWriteForwardsDefaultLayout()
    {
        Device device;
        auto target = device.i2cDevice(0x48);

        libeasymcp2221::qt::writeRegister(
            target,
            0x23,
            QByteArray::fromHex("a1b2"));

        QCOMPARE(
            libeasymcp2221_test::lastI2cRegister(),
            static_cast<std::uint32_t>(0x23));
        QCOMPARE(
            libeasymcp2221_test::lastI2cRegisterWidth(),
            0);
        QCOMPARE(
            libeasymcp2221_test::lastI2cByteOrder(),
            MCP2221_I2C_BYTE_ORDER_DEFAULT);

        const auto& data =
            libeasymcp2221_test::lastI2cWriteData();
        QCOMPARE(
            QByteArray(
                reinterpret_cast<const char*>(data.data()),
                static_cast<qsizetype>(data.size())),
            QByteArray::fromHex("a1b2"));
    }

    void i2cRegisterWriteForwardsExplicitLayout()
    {
        Device device;
        auto target = device.i2cDevice(0x48);

        libeasymcp2221::qt::writeRegister(
            target,
            0x1234,
            QByteArray::fromHex("c0ffee"),
            libeasymcp2221::RegisterWidth::Bits16,
            libeasymcp2221::ByteOrder::LittleEndian);

        QCOMPARE(
            libeasymcp2221_test::lastI2cRegister(),
            static_cast<std::uint32_t>(0x1234));
        QCOMPARE(
            libeasymcp2221_test::lastI2cRegisterWidth(),
            2);
        QCOMPARE(
            libeasymcp2221_test::lastI2cByteOrder(),
            MCP2221_I2C_BYTE_ORDER_LITTLE);

        const auto& data =
            libeasymcp2221_test::lastI2cWriteData();
        QCOMPARE(
            QByteArray(
                reinterpret_cast<const char*>(data.data()),
                static_cast<qsizetype>(data.size())),
            QByteArray::fromHex("c0ffee"));
    }

    void smbusBlockWriteForwardsPayload()
    {
        Device device;
        auto target = device.smbusDevice(0x5A);

        libeasymcp2221::qt::writeBlockData(
            target,
            0x21,
            QByteArray::fromHex("102030"));

        QCOMPARE(
            libeasymcp2221_test::lastSmbusCommand(),
            static_cast<std::uint8_t>(0x21));

        const auto& data =
            libeasymcp2221_test::lastSmbusWriteData();
        QCOMPARE(
            QByteArray(
                reinterpret_cast<const char*>(data.data()),
                static_cast<qsizetype>(data.size())),
            QByteArray::fromHex("102030"));
    }

    void smbusBlockProcessCallForwardsPayload()
    {
        Device device;
        auto target = device.smbusDevice(0x5A);

        QCOMPARE(
            libeasymcp2221::qt::blockProcessCall(
                target,
                0x22,
                QByteArray::fromHex("4455")),
            QByteArray::fromHex("55"));

        QCOMPARE(
            libeasymcp2221_test::lastSmbusCommand(),
            static_cast<std::uint8_t>(0x22));

        const auto& data =
            libeasymcp2221_test::lastSmbusWriteData();
        QCOMPARE(
            QByteArray(
                reinterpret_cast<const char*>(data.data()),
                static_cast<qsizetype>(data.size())),
            QByteArray::fromHex("4455"));
    }

    void smbusI2cBlockWriteForwardsPayload()
    {
        Device device;
        auto target = device.smbusDevice(0x5A);

        libeasymcp2221::qt::writeI2cBlockData(
            target,
            0x23,
            QByteArray::fromHex("deadbeef"));

        QCOMPARE(
            libeasymcp2221_test::lastSmbusCommand(),
            static_cast<std::uint8_t>(0x23));

        const auto& data =
            libeasymcp2221_test::lastSmbusWriteData();
        QCOMPARE(
            QByteArray(
                reinterpret_cast<const char*>(data.data()),
                static_cast<qsizetype>(data.size())),
            QByteArray::fromHex("deadbeef"));
    }

    void i2cRegisterReadForwardsExplicitLayout()
    {
        Device device;
        auto target = device.i2cDevice(0x48);

        QCOMPARE(
            libeasymcp2221::qt::readRegister(
                target,
                0x1234,
                2,
                libeasymcp2221::RegisterWidth::Bits16,
                libeasymcp2221::ByteOrder::LittleEndian),
            QByteArray(2, static_cast<char>(0x3C)));

        QCOMPARE(
            libeasymcp2221_test::lastI2cRegister(),
            static_cast<std::uint32_t>(0x1234));
        QCOMPARE(
            libeasymcp2221_test::lastI2cRegisterWidth(),
            2);
        QCOMPARE(
            libeasymcp2221_test::lastI2cByteOrder(),
            MCP2221_I2C_BYTE_ORDER_LITTLE);
    }

    void smbusRejectsNegativeSize()
    {
        Device device;
        auto target = device.smbusDevice(0x5A);

        QVERIFY_EXCEPTION_THROWN(
            (void)libeasymcp2221::qt::readI2cBlockData(
                target,
                0x03,
                -1),
            std::invalid_argument);
    }

    void emptyWriteSemanticsMatchCore()
    {
        Device device;

        auto i2c = device.i2cDevice(0x48);
        QVERIFY_EXCEPTION_THROWN(
            libeasymcp2221::qt::write(i2c, {}),
            libeasymcp2221::Error);

        libeasymcp2221::qt::writeRegister(i2c, 0x11, {});
        QVERIFY(
            libeasymcp2221_test::lastI2cWriteData().empty());

        auto smbus = device.smbusDevice(0x5A);
        libeasymcp2221::qt::writeBlockData(
            smbus,
            0x20,
            {});
        QVERIFY(
            libeasymcp2221_test::lastSmbusWriteData().empty());

        libeasymcp2221::qt::writeI2cBlockData(
            smbus,
            0x21,
            {});
        QVERIFY(
            libeasymcp2221_test::lastSmbusWriteData().empty());
    }

    void qtFunctionsPropagateCoreErrors()
    {
        Device device;

        auto i2c = device.i2cDevice(0x48);
        libeasymcp2221_test::failNext(MCP2221_ERR_TIMEOUT);
        QVERIFY_EXCEPTION_THROWN(
            (void)libeasymcp2221::qt::read(i2c, 1),
            libeasymcp2221::Error);

        auto smbus = device.smbusDevice(0x5A);
        libeasymcp2221_test::failNext(MCP2221_ERR_TIMEOUT);
        QVERIFY_EXCEPTION_THROWN(
            libeasymcp2221::qt::writeBlockData(
                smbus,
                0x20,
                QByteArray::fromHex("01")),
            libeasymcp2221::Error);
    }

    void qtMetaTypesAreDeclared()
    {
        QVERIFY(QMetaType::fromType<Pin>().isValid());
        QVERIFY(
            QMetaType::fromType<
                libeasymcp2221::GpioEdge>().isValid());
        QVERIFY(QMetaType::fromType<GpioEvent>().isValid());
        QVERIFY(
            QMetaType::fromType<
                libeasymcp2221::ErrorCode>().isValid());
        QVERIFY(QMetaType::fromType<ErrorInfo>().isValid());
    }

    void gpioMonitorTimerPollsAutomatically()
    {
        qRegisterMetaType<GpioEvent>();

        Device device;
        GpioMonitor monitor(device.gpioPoller());
        monitor.setInterval(std::chrono::milliseconds{1});

        QSignalSpy eventSpy(
            &monitor,
            &GpioMonitor::gpioEvent);

        libeasymcp2221_test::queueGpioEvent(
            0,
            MCP2221_GPIO_EVENT_RISE);

        monitor.start();
        QTRY_COMPARE(eventSpy.count(), 1);
        monitor.stop();

        const auto event =
            qvariant_cast<GpioEvent>(eventSpy.at(0).at(0));
        QCOMPARE(event.pin, Pin::GP0);
        QCOMPARE(
            event.edge,
            libeasymcp2221::GpioEdge::Rising);
    }

    void gpioMonitorDefaults()
    {
        Device device;
        GpioMonitor monitor(device.gpioPoller());

        QCOMPARE(
            monitor.interval(),
            GpioMonitor::DefaultInterval);
        QCOMPARE(
            monitor.maxEventsPerPoll(),
            GpioMonitor::DefaultMaxEventsPerPoll);
        QVERIFY(!monitor.isActive());
    }

    void gpioMonitorStartStopSignals()
    {
        Device device;
        GpioMonitor monitor(device.gpioPoller());

        QSignalSpy startedSpy(&monitor, &GpioMonitor::started);
        QSignalSpy stoppedSpy(&monitor, &GpioMonitor::stopped);

        monitor.start();
        QVERIFY(monitor.isActive());
        QCOMPARE(startedSpy.count(), 1);

        monitor.start();
        QCOMPARE(startedSpy.count(), 1);

        monitor.stop();
        QVERIFY(!monitor.isActive());
        QCOMPARE(stoppedSpy.count(), 1);

        monitor.stop();
        QCOMPARE(stoppedSpy.count(), 1);
    }

    void gpioMonitorPollWithoutEvents()
    {
        qRegisterMetaType<GpioEvent>();
        qRegisterMetaType<Pin>();

        Device device;
        GpioMonitor monitor(device.gpioPoller());

        QSignalSpy eventSpy(&monitor, &GpioMonitor::gpioEvent);
        QSignalSpy risingSpy(&monitor, &GpioMonitor::risingEdge);
        QSignalSpy fallingSpy(&monitor, &GpioMonitor::fallingEdge);

        monitor.pollOnce();

        QCOMPARE(eventSpy.count(), 0);
        QCOMPARE(risingSpy.count(), 0);
        QCOMPARE(fallingSpy.count(), 0);
    }

    void gpioMonitorReportsPollingError()
    {
        qRegisterMetaType<ErrorInfo>();

        Device device;
        GpioMonitor monitor(device.gpioPoller());

        QSignalSpy errorSpy(&monitor, &GpioMonitor::errorOccurred);

        libeasymcp2221_test::failNext(MCP2221_ERR_TIMEOUT);
        monitor.pollOnce();

        QCOMPARE(errorSpy.count(), 1);

        const auto error =
            qvariant_cast<ErrorInfo>(errorSpy.at(0).at(0));

        QCOMPARE(
            error.code,
            libeasymcp2221::ErrorCode::Timeout);
        QCOMPARE(
            error.nativeCode,
            static_cast<int>(MCP2221_ERR_TIMEOUT));
        QVERIFY(!error.message.isEmpty());
    }

    void gpioMonitorErrorDoesNotStopTimer()
    {
        Device device;
        GpioMonitor monitor(device.gpioPoller());

        monitor.start();
        libeasymcp2221_test::failNext(MCP2221_ERR_TIMEOUT);
        monitor.pollOnce();

        QVERIFY(monitor.isActive());
        monitor.stop();
    }


    void gpioMonitorEmitsEdgeSignals()
    {
        qRegisterMetaType<GpioEvent>();
        qRegisterMetaType<Pin>();

        Device device;
        GpioMonitor monitor(device.gpioPoller());

        QSignalSpy eventSpy(&monitor, &GpioMonitor::gpioEvent);
        QSignalSpy risingSpy(&monitor, &GpioMonitor::risingEdge);
        QSignalSpy fallingSpy(&monitor, &GpioMonitor::fallingEdge);

        libeasymcp2221_test::queueGpioEvent(
            1,
            MCP2221_GPIO_EVENT_RISE,
            12.5,
            12.0);
        libeasymcp2221_test::queueGpioEvent(
            2,
            MCP2221_GPIO_EVENT_FALL,
            13.0,
            12.5);

        monitor.pollOnce();

        QCOMPARE(eventSpy.count(), 2);
        QCOMPARE(risingSpy.count(), 1);
        QCOMPARE(fallingSpy.count(), 1);

        const auto risingEvent =
            qvariant_cast<GpioEvent>(eventSpy.at(0).at(0));
        QCOMPARE(risingEvent.pin, Pin::GP1);
        QCOMPARE(risingEvent.edge, libeasymcp2221::GpioEdge::Rising);
        QCOMPARE(risingEvent.time, 12.5);
        QCOMPARE(risingEvent.previousTime, 12.0);

        QCOMPARE(
            qvariant_cast<Pin>(risingSpy.at(0).at(0)),
            Pin::GP1);
        QCOMPARE(
            qvariant_cast<Pin>(fallingSpy.at(0).at(0)),
            Pin::GP2);
    }

    void gpioMonitorFilterSuppressesUnselectedEdges()
    {
        qRegisterMetaType<GpioEvent>();

        Device device;
        GpioMonitor monitor(device.gpioPoller());

        GpioEventFilter filter;
        filter.rising[1] = true;
        monitor.setFilter(filter);

        QSignalSpy eventSpy(&monitor, &GpioMonitor::gpioEvent);

        libeasymcp2221_test::queueGpioEvent(
            1,
            MCP2221_GPIO_EVENT_FALL);
        libeasymcp2221_test::queueGpioEvent(
            2,
            MCP2221_GPIO_EVENT_RISE);
        libeasymcp2221_test::queueGpioEvent(
            1,
            MCP2221_GPIO_EVENT_RISE);

        monitor.pollOnce();

        QCOMPARE(eventSpy.count(), 1);
        const auto event =
            qvariant_cast<GpioEvent>(eventSpy.at(0).at(0));
        QCOMPARE(event.pin, Pin::GP1);
        QCOMPARE(event.edge, libeasymcp2221::GpioEdge::Rising);
    }

    void gpioMonitorDiscardsEventsBeyondMaximum()
    {
        qRegisterMetaType<GpioEvent>();

        Device device;
        GpioMonitor monitor(device.gpioPoller());
        monitor.setMaxEventsPerPoll(1);

        QSignalSpy eventSpy(&monitor, &GpioMonitor::gpioEvent);

        libeasymcp2221_test::queueGpioEvent(
            0,
            MCP2221_GPIO_EVENT_RISE);
        libeasymcp2221_test::queueGpioEvent(
            1,
            MCP2221_GPIO_EVENT_RISE);

        monitor.pollOnce();
        QCOMPARE(eventSpy.count(), 1);

        monitor.pollOnce();
        QCOMPARE(eventSpy.count(), 1);
    }

    void gpioMonitorRecoversAfterPollingError()
    {
        qRegisterMetaType<GpioEvent>();

        Device device;
        GpioMonitor monitor(device.gpioPoller());

        QSignalSpy eventSpy(&monitor, &GpioMonitor::gpioEvent);
        QSignalSpy errorSpy(&monitor, &GpioMonitor::errorOccurred);

        libeasymcp2221_test::failNext(MCP2221_ERR_TIMEOUT);
        monitor.pollOnce();

        QCOMPARE(errorSpy.count(), 1);
        QCOMPARE(eventSpy.count(), 0);

        libeasymcp2221_test::queueGpioEvent(
            3,
            MCP2221_GPIO_EVENT_RISE);
        monitor.pollOnce();

        QCOMPARE(errorSpy.count(), 1);
        QCOMPARE(eventSpy.count(), 1);

        const auto event =
            qvariant_cast<GpioEvent>(eventSpy.at(0).at(0));
        QCOMPARE(event.pin, Pin::GP3);
        QCOMPARE(event.edge, libeasymcp2221::GpioEdge::Rising);
    }

    void gpioMonitorForwardsFilter()
    {
        Device device;
        GpioMonitor monitor(device.gpioPoller());

        GpioEventFilter filter;
        filter.rising[1] = true;
        monitor.setFilter(filter);

        QCOMPARE(
            libeasymcp2221_test::lastGpioFilterMask(),
            static_cast<std::uint16_t>(
                MCP2221_GPIO_POLL_MASK_RISE(1)));

        monitor.clearFilter();
        QCOMPARE(
            libeasymcp2221_test::lastGpioFilterMask(),
            static_cast<std::uint16_t>(0));
    }
};

}  // namespace

QTEST_GUILESS_MAIN(QtIntegrationTest)

#include "test_qt.moc"
