#include "Config.hpp"

#include "PPM.hpp"

PPM::PPM(Mode mode)
    : ready(false), mode(mode), width(0), height(0)
{
}

bool PPM::create(std::int32_t width, std::int32_t height, Format format)
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

bool PPM::isReady() const
{
    return ready;
}

bool PPM::convert(Format format)
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

bool PPM::destroy()
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

PPM::Mode PPM::getMode() const
{
    return mode;
}

PPM::Format PPM::getFormat() const
{
    return format;
}

std::uint32_t PPM::getWidth() const
{
    return width;
}

std::uint32_t PPM::getHeight() const
{
    return height;
}

std::uint32_t PPM::getSize() const
{
    return width * height * getChannels(format);
}

void * PPM::getData() const
{
    return (void*)data.data();
}

bool PPM::operator==(const PPM & other)
{
    return std::equal(data.begin(), data.end(), other.data.begin());
}

std::size_t PPM::getChannels(Format format) const
{
    std::size_t result;

    switch (format)
    {
        case PPM::Format::INTENSITY: result = 1; break;
        case PPM::Format::LUMINANCE: result = 1; break;
        case PPM::Format::RGB: result = 3; break;
        case PPM::Format::RGBA: result = 4; break;
        default: throw std::runtime_error("Unsupported image format!");
    }

    return result;
}

std::ostream& operator<<(std::ostream& output, const PPM& image)
{
    if (!image.isReady())
    {
        return output;
    }

    std::int32_t num;
    std::int32_t max;

    switch (image.getFormat())
    {
        case PPM::Format::INTENSITY: 
        {
            num = 1;
            max = 1;
        }
        break;

        case PPM::Format::LUMINANCE: 
        {
            num = 2;
            max = std::numeric_limits<std::uint8_t>::max();
        }
        break;

        case PPM::Format::RGB:
        {
            num = 3;
            max = std::numeric_limits<std::uint8_t>::max();
        }
        break;

        default: throw std::runtime_error("Unsuported format to save PPM file!");
    }

    if (image.getMode() == PPM::Mode::Binary)
    {
        num += 3;
    }

    output << 'P' << std::to_string(num) << std::endl
        << image.getWidth() << " " << image.getHeight() << std::endl
        << max << std::endl;

    if (image.getMode() == PPM::Mode::Binary)
    {
        output.write(static_cast<char*>(image.getData()), image.getSize());
    }
    else
    {
        std::size_t count = 0;
        std::size_t size = image.getSize();
        unsigned* data = static_cast<unsigned*>(image.getData());

        while (count < size)
        {
            output << data[count++] << ' ';

            if (count % 18 == 0)
            {
                output << '\n';
            }
        }
    }
    
    return output;
}

std::istream& operator>>(std::istream& input, PPM& image)
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

    PPM::Format format;

    switch (num)
    {
        case 6:
        case 3: format = PPM::Format::RGB; break;

        case 5:
        case 2: format = PPM::Format::LUMINANCE; break;

        case 4:
        case 1: format = PPM::Format::INTENSITY; break;

        default: throw std::runtime_error("Invalid PPM image format!");
    }

    if (num > 3)
    {
        image = PPM(PPM::Mode::Binary);
    }

    if (!image.create(std::stoi(width), std::stoi(height), format))
    {
        return input;
    }

    if (image.getMode() == PPM::Mode::Binary)
    {
        input.read(static_cast<char*>(image.getData()), image.getSize());
    }
    else
    {
        unsigned byte;
        std::size_t count = 0;
        std::size_t size = image.getSize();
        std::uint8_t* data = static_cast<std::uint8_t*>(image.getData());

        while (!input.eof() && count < size)
        {
            input >> std::skipws >> byte;

            data[count++] = byte;
        }
    }
    
    return input;
}
