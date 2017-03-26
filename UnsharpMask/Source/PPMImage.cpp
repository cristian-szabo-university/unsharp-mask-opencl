#include "PPMImage.hpp"

PPMImage::PPMImage(Mode mode)
    : ready(false), mode(mode), width(0), height(0)
{
}

bool PPMImage::create(std::int32_t width, std::int32_t height, Format format)
{
    if (isReady())
    {
        return false;
    }

    if (width < 1 || height < 1)
    {
        return false;
    }

    this->format = format;

    this->width = width;

    this->height = height;

    std::size_t channels = getChannels(format);

    data.resize(width * height * channels);

    ready = !ready;

    return true;
}

bool PPMImage::isReady() const
{
    return ready;
}

bool PPMImage::convert(Format format)
{
    if (!isReady())
    {
        return false;
    }

    if (this->format == format)
    {
        return true;
    }

    std::size_t size = width * height;
    std::size_t new_channels = getChannels(format);
    std::size_t old_channels = getChannels(this->format);
    std::vector<std::uint8_t> new_data;
    new_data.resize(size * new_channels);

    std::size_t channels = std::min(old_channels, new_channels);

    for (std::size_t i = 0; i < size; i++)
    {
        for (std::size_t j = 0; j < channels; j++)
        {
            new_data[i * new_channels + j] = data[i * old_channels + j];
        }
    }

    data.clear();

    this->format = format;

    data.assign(new_data.begin(), new_data.end());

    return true;
}

bool PPMImage::destroy()
{
    if (!isReady())
    {
        return false;
    }

    width = 0;

    height = 0;

    format = Format::Invalid;

    data.clear();

    ready = !ready;

    return true;
}

std::uint32_t PPMImage::getWidth() const
{
    return width;
}

std::uint32_t PPMImage::getHeight() const
{
    return height;
}

std::uint32_t PPMImage::getSize() const
{
    return width * height * getChannels(format);
}

void * PPMImage::getData() const
{
    return (void*)data.data();
}

bool PPMImage::operator==(const PPMImage & other)
{
    return std::equal(data.begin(), data.end(), other.data.begin());
}

std::size_t PPMImage::getChannels(Format format) const
{
    std::size_t result;

    switch (format)
    {
        case PPMImage::Format::INTENSITY: result = 1; break;
        case PPMImage::Format::LUMINANCE: result = 1; break;
        case PPMImage::Format::RGB: result = 3; break;
        case PPMImage::Format::RGBA: result = 4; break;
        default: throw std::runtime_error("Unsupported image format!");
    }

    return result;
}

std::ostream& operator<<(std::ostream& output, const PPMImage& image)
{
    if (!image.isReady())
    {
        return output;
    }

    std::int32_t num;
    std::int32_t max;

    switch (image.format)
    {
        case PPMImage::Format::INTENSITY: 
        {
            num = 1;
            max = 1;
        }
        break;

        case PPMImage::Format::LUMINANCE: 
        {
            num = 2;
            max = std::numeric_limits<std::uint8_t>::max();
        }
        break;

        case PPMImage::Format::RGB:
        {
            num = 3;
            max = std::numeric_limits<std::uint8_t>::max();
        }
        break;

        default: throw std::runtime_error("Unsuported format to save PPM file!");
    }

    if (image.mode == PPMImage::Mode::Binary)
    {
        num += 3;
    }

    output << 'P' << std::to_string(num) << std::endl
        << image.width << " " << image.height << std::endl
        << max << std::endl;

    if (image.mode == PPMImage::Mode::Binary)
    {
        output.write((char*)&image.data[0], image.getSize());
    }
    else
    {
        unsigned data;
        std::size_t count = 0;
        std::size_t size = image.getSize();

        while (count < size)
        {
            data = image.data[count++];

            output << data << ' ';

            if (count % 18 == 0)
            {
                output << '\n';
            }
        }
    }

    return output;
}

std::istream& operator>>(std::istream& input, PPMImage& image)
{
    std::string magic;
    input >> magic;

    if (magic.size() != 2 && magic[0] != 'P')
    {
        throw std::runtime_error("Invalid PPM header file!");
    }

    std::uint32_t num = std::atoi(&magic[1]);
    
    std::string width, height;
    input >> width >> height;

    std::string max;
    input >> max;

    PPMImage::Format format;

    switch (num)
    {
        case 6:
        case 3: format = PPMImage::Format::RGB; break;

        case 5:
        case 2: format = PPMImage::Format::LUMINANCE; break;

        case 4:
        case 1: format = PPMImage::Format::INTENSITY; break;

        default: throw std::runtime_error("Invalid PPM image format!");
    }

    if (num > 3)
    {
        image.mode = PPMImage::Mode::Binary;
    }
    else
    {
        image.mode = PPMImage::Mode::ASCII;
    }

    if (!image.create(std::stoi(width), std::stoi(height), format))
    {
        return input;
    }

    if (image.mode == PPMImage::Mode::Binary)
    {
        input.read((char*)&image.data[0], image.getSize());
    }
    else
    {
        unsigned data;
        std::size_t count = 0;
        std::size_t size = image.getSize();

        while (!input.eof() && count < size)
        {
            input >> std::skipws >> data;

            image.data[count++] = data;
        }
    }

    return input;
}
