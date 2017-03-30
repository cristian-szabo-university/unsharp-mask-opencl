#include "Config.hpp"

#include "PPM.hpp"

PPM::PPM(Mode mode)
    : ready(false), mode(mode), width(0), height(0), channels(0)
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

    channels = getChannels(format);

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

    std::int32_t size = width * height;
    std::int32_t new_channels = getChannels(format);
    std::vector<std::uint8_t> new_data(size * new_channels, 0);

    std::int32_t steps = std::min(channels, new_channels);

    for (std::int32_t i = 0; i < size; i++)
    {
        for (std::int32_t j = 0; j < steps; j++)
        {
            new_data[i * new_channels + j] = data[i * channels + j];
        }
    }

    data.clear();

    data.assign(new_data.begin(), new_data.end());

    this->format = format;

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

std::int32_t PPM::getGLFormat() const
{
    GLint result;

    switch (format)
    {
        case Format::R: result = GL_R; break;
        case Format::RG: result = GL_RG; break;
        case Format::RGB: result = GL_RGB; break;
        case Format::RGBA: result = GL_RGBA; break;
        default: throw std::runtime_error("Invalid image format!");
    }

    return result;
}

std::uint32_t PPM::getGLInternalFormat() const
{
    GLenum result;

    switch (format)
    {
        case Format::R: result = GL_R8; break;
        case Format::RG: result = GL_RG8; break;
        case Format::RGB: result = GL_RGB8; break;
        case Format::RGBA: result = GL_RGBA8; break;
        default: throw std::runtime_error("Invalid image format!");
    }

    return result;
}

std::size_t PPM::getChannels() const
{
    return channels;
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
    return width * height * channels;
}

void* PPM::getData() const
{
    return (void*)data.data();
}

void PPM::setPixel(std::uint32_t x, std::uint32_t y, const std::vector<std::uint8_t>& pixel)
{
    const std::uint32_t offset = (y * width + x) * channels;

    std::copy(pixel.begin(), pixel.end(), data.begin() + offset);
}

std::vector<std::uint8_t> PPM::getPixel(std::uint32_t x, std::uint32_t y) const
{
    const std::uint32_t offset = (y * width + x) * channels;

    return std::vector<std::uint8_t>(
        data.begin() + offset,
        data.begin() + offset + channels);
}

bool PPM::operator==(const PPM & other)
{
    return std::equal(data.begin(), data.end(), other.data.begin());
}

std::int32_t PPM::getChannels(Format format) const
{
    std::int32_t result;

    switch (format)
    {
        case PPM::Format::R: result = 1; break;
        case PPM::Format::RG: result = 2; break;
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

    if (image.getFormat() == PPM::Format::RGBA ||
        image.getFormat() == PPM::Format::RG)
    {
        std::cerr << "Error: Image format not compatible with PPM format!\n";
        std::cerr << "Convert to a suitable fromat before writting!\n";

        return output;
    }
   
    std::int32_t max = std::numeric_limits<std::uint8_t>::max();

    std::int32_t ppm_id;

    switch (image.getFormat())
    {
        case PPM::Format::R: 
        {
            ppm_id = 1;
        }
        break;

        case PPM::Format::RGB:
        {
            ppm_id = 3;
        }
        break;

        default: throw std::runtime_error("Unsuported format to save PPM file!");
    }

    if (image.getMode() == PPM::Mode::Binary)
    {
        ppm_id += 3;
    }

    output << 'P' << std::to_string(ppm_id) << std::endl
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
    if (image.isReady())
    {
        return input;
    }

    std::string magic;
    input >> magic;

    if (magic.size() != 2 && magic[0] != 'P')
    {
        throw std::runtime_error("Invalid PPM header file!");
    }

    std::uint32_t ppm_id = std::atoi(&magic[1]);
    
    std::string width, height;
    input >> width >> height;

    std::string max;
    input >> max;

    PPM::Format format;

    switch (ppm_id)
    {
        case 6:
        case 3: format = PPM::Format::RGB; break;

        case 5:
        case 4:
        case 2: 
        case 1: format = PPM::Format::R; break;
        
        default: throw std::runtime_error("Invalid PPM image format!");
    }

    if (ppm_id > 3)
    {
        image = PPM(PPM::Mode::Binary);
    }

    if (!image.create(std::stoi(width), std::stoi(height), format))
    {
        return input;
    }

    std::uint64_t read_bytes = 0;

    if (image.getMode() == PPM::Mode::Binary)
    {
        input.read(static_cast<char*>(image.getData()), image.getSize());

        read_bytes = input.gcount();
    }
    else
    {     
        std::uint8_t* data = static_cast<std::uint8_t*>(image.getData());

        unsigned byte;

        while (!input.eof())
        {
            input >> std::skipws >> byte;

            data[read_bytes++] = byte;
        }

        --read_bytes;
    }

    if (read_bytes != image.getSize())
    {
        image.destroy();
    }
    
    return input;
}
