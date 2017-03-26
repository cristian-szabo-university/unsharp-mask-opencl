#pragma once

#include <vector>
#include <cstdint>
#include <sstream>
#include <algorithm>
#include <functional>

class PPMImage
{
public:

    enum class Mode
    {
        Invalid,
        ASCII,
        Binary,
        MaxCount
    };

    enum class Format
    {
        Invalid,
        INTENSITY,
        LUMINANCE,
        RGB,
        RGBA,
        MaxCount
    };

    PPMImage(Mode mode = Mode::ASCII);

    bool create(std::int32_t width, std::int32_t height, Format format = Format::RGB);

    bool isReady() const;

    bool convert(Format format);

    bool destroy();

    std::uint32_t getWidth() const;

    std::uint32_t getHeight() const;

    std::size_t getSize() const;

    void* getData() const;

    bool operator==(const PPMImage& other);

    friend std::ostream &operator<<(std::ostream& output, const PPMImage& other);

    friend std::istream &operator>>(std::istream& input, PPMImage& other);

private:

    bool ready;

    Mode mode;

    std::int32_t width;

    std::int32_t height;

    Format format;

    std::vector<std::uint8_t> data;

    std::size_t getChannels(Format format) const;

};
