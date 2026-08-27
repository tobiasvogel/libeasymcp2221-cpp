#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string_view>
#include <vector>

#include <libeasymcp2221++/libeasymcp2221++.h>

using namespace libeasymcp2221;

namespace {

constexpr std::uint8_t kAddress = 0x3C;
constexpr int kWidth = 128;
constexpr int kHeight = 32;
constexpr int kPages = kHeight / 8;
constexpr int kGlyphWidth = 6;
constexpr int kGlyphHeight = 8;

using Glyph = std::array<std::uint8_t, kGlyphWidth>;
using Framebuffer = std::array<std::uint8_t, kWidth * kPages>;

constexpr Glyph glyph(char c)
{
    switch (c) {
    case ' ':
        return {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    case '!':
        return {0x00, 0x00, 0x5F, 0x00, 0x00, 0x00};
    case 'H':
        return {0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00};
    case 'W':
        return {0x7F, 0x20, 0x18, 0x20, 0x7F, 0x00};
    case 'd':
        return {0x38, 0x44, 0x44, 0x48, 0x7F, 0x00};
    case 'e':
        return {0x38, 0x54, 0x54, 0x54, 0x18, 0x00};
    case 'l':
        return {0x00, 0x41, 0x7F, 0x40, 0x00, 0x00};
    case 'o':
        return {0x38, 0x44, 0x44, 0x44, 0x38, 0x00};
    case 'r':
        return {0x7C, 0x08, 0x04, 0x04, 0x08, 0x00};
    default:
        return {};
    }
}

void command(I2cDevice& display, std::uint8_t value)
{
    display.write(std::vector<std::uint8_t>{0x00, value});
}

void initialize(I2cDevice& display)
{
    constexpr std::array<std::uint8_t, 26> commands{
        0xAE,
        0xD5, 0x80,
        0xA8, static_cast<std::uint8_t>(kHeight - 1),
        0xD3, 0x00,
        0x40,
        0x8D, 0x14,
        0x20, 0x00,
        0xA1,
        0xC8,
        0xDA, 0x02,
        0x81, 0x8F,
        0xD9, 0xF1,
        0xDB, 0x40,
        0xA4,
        0xA6,
        0x2E,
        0xAF
    };

    for (const auto value : commands) {
        command(display, value);
    }
}

void drawChar(Framebuffer& framebuffer, int x, int y, char c)
{
    const auto bitmap = glyph(c);

    for (int column = 0; column < kGlyphWidth; ++column) {
        for (int row = 0; row < kGlyphHeight; ++row) {
            if ((bitmap[static_cast<std::size_t>(column)] & (1u << row)) == 0) {
                continue;
            }

            const int px = x + column;
            const int py = y + row;
            if (px < 0 || px >= kWidth || py < 0 || py >= kHeight) {
                continue;
            }

            const auto index =
                static_cast<std::size_t>((py / 8) * kWidth + px);
            framebuffer[index] |= static_cast<std::uint8_t>(1u << (py % 8));
        }
    }
}

void drawText(Framebuffer& framebuffer, int x, int y, std::string_view text)
{
    for (const char c : text) {
        drawChar(framebuffer, x, y, c);
        x += kGlyphWidth;
    }
}

void flush(I2cDevice& display, const Framebuffer& framebuffer)
{
    for (int page = 0; page < kPages; ++page) {
        command(display, static_cast<std::uint8_t>(0xB0 + page));
        command(display, 0x00);
        command(display, 0x10);

        for (int column = 0; column < kWidth; column += 16) {
            std::vector<std::uint8_t> chunk;
            chunk.reserve(17);
            chunk.push_back(0x40);

            const auto begin =
                framebuffer.begin() + page * kWidth + column;
            chunk.insert(chunk.end(), begin, begin + 16);
            display.write(chunk);
        }
    }
}

}  // namespace

int main()
{
    try {
        Device device;
        device.setI2cSpeed(100000);

        I2cDeviceOptions options{};
        options.force = true;
        options.speedHz = 100000;
        options.registerWidth = RegisterWidth::Bits8;
        options.byteOrder = ByteOrder::BigEndian;

        auto display = device.i2cDevice(kAddress, options);
        initialize(display);

        Framebuffer framebuffer{};
        drawText(framebuffer, 0, 0, "Hello World!");
        flush(display, framebuffer);

        std::cout << "Done.\n";
        return 0;
    }
    catch (const Error& error) {
        std::cerr << "SSD1306 example failed: " << error.what() << '\n';
        return 1;
    }
}
