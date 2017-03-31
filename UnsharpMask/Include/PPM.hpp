#pragma once

class UNSHARP_MASK_PUBLIC PPM
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
        R,
        RG,
        RGB,
        RGBA,
        MaxCount
    };

    PPM(Mode mode = Mode::ASCII);

    bool create(std::int32_t width, std::int32_t height, std::uint32_t internal_format);

    bool create(std::int32_t width, std::int32_t height, Format format = Format::RGB);

    bool isReady() const;

    bool convert(Format format);

    bool destroy();

    Mode getMode() const;

    Format getFormat() const;

    std::int32_t getGLFormat() const;

    cl_channel_order getCLFormat() const;

    std::uint32_t getGLInternalFormat() const;

    std::size_t getChannels() const;

    std::uint32_t getWidth() const;

    std::uint32_t getHeight() const;

    std::size_t getSize() const;

    void* getData() const;

    void setPixel(std::uint32_t x, std::uint32_t y, const std::vector<std::uint8_t>& pixel);

    std::vector<std::uint8_t> getPixel(std::uint32_t x, std::uint32_t y) const;

    bool operator==(const PPM& other);

private:

    bool ready;

    Mode mode;

    Format format;

    std::int32_t width;

    std::int32_t height;

    std::int32_t channels;

    std::vector<std::uint8_t> data;

    std::int32_t getChannels(Format format) const;

};

UNSHARP_MASK_PUBLIC std::ostream& operator<<(std::ostream& output, const PPM& other);

UNSHARP_MASK_PUBLIC std::istream& operator>>(std::istream& input, PPM& other);