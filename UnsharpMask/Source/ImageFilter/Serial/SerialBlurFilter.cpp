#include "Config.hpp"

#include "ImageFilter\Serial\SerialBlurFilter.hpp"

#include "ImageProcess\Serial\SerialBlurSharpProcess.hpp"

#include "GL\glew.h"

#include "Utility.hpp"

SerialBlurFilter::SerialBlurFilter()
{
}

SerialBlurFilter::~SerialBlurFilter()
{
}

bool SerialBlurFilter::onLoad(std::shared_ptr<ImageProcess> proc)
{
    std::shared_ptr<SerialBlurSharpProcess> cast_proc = std::dynamic_pointer_cast<SerialBlurSharpProcess>(proc);

    if (!cast_proc)
    {
        return false;
    }

    radius = cast_proc->getRadius();

    return true;
}

void SerialBlurFilter::onBefore(std::uint32_t glTexId)
{
    glBindTexture(GL_TEXTURE_2D, glTexId);

    if (previous.isReady())
    {
        GLint internalFormat;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

        GLenum format;

        switch (internalFormat)
        {
            case GL_RGBA8: format = GL_RGBA; break;
            case GL_RGB8: format = GL_RGB; break;
            case GL_R8: format = GL_R; break;
            default: throw std::runtime_error("Invalid image format!");
        }

        glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, previous.getData());
    }

    if (result.isReady())
    {
        result.destroy();
    }
}

std::uint64_t SerialBlurFilter::onApply(const PPM & image)
{
    result.create(image.getWidth(), image.getHeight(), image.getFormat());

    PPM input = image;

    if (!previous.isReady())
    {
        previous.create(image.getWidth(), image.getHeight(), image.getFormat());
    }
    else
    {
        input = previous;
    }

    std::int32_t channels = image.getChannels();
    std::int32_t width = image.getWidth(), height = image.getHeight();
    float samples = std::powf(radius * 2.0f + 1.0f, 2.0f);
    std::uint8_t* in_data = static_cast<std::uint8_t*>(input.getData());
    std::uint8_t* out_data = static_cast<std::uint8_t*>(result.getData());

    auto start_time = std::chrono::high_resolution_clock::now();

    for (std::int32_t y = 0; y < height; ++y)
    {
        for (std::int32_t x = 0; x < width; ++x)
        {
            pixelAverage(x, y, width, height, in_data, out_data, channels, samples);
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
}

void SerialBlurFilter::onAfter()
{
    GLint internalFormat;
    GLenum format;

    switch (result.getFormat())
    {
        case PPM::Format::RGBA:
            internalFormat = GL_RGBA8;
            format = GL_RGBA;
            break;

        case PPM::Format::RGB:
            internalFormat = GL_RGB8;
            format = GL_RGB;
            break;

        case PPM::Format::INTENSITY:
        case PPM::Format::LUMINANCE:
            internalFormat = GL_R8;
            format = GL_R;
            break;

        default: throw std::runtime_error("Invalid image format!");
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, result.getWidth(), result.getHeight(), 0, format, GL_UNSIGNED_BYTE, result.getData());

    glBindTexture(GL_TEXTURE_2D, 0);
}

void SerialBlurFilter::pixelAverage(
    std::int32_t mx, std::int32_t my,
    std::int32_t width, std::int32_t height,
    std::uint8_t* input, std::uint8_t* output,
    const std::int32_t channels, float samples)
{
    float accum[4] = { 0.0f };

    for (int y = -radius; y <= radius; ++y)
    {
        int row = my + y;
        row = row < 0 ? 0 : row >= height ? height - 1 : row;
        row *= width * channels;

        for (int x = -radius; x <= radius; ++x)
        {
            int column = mx + x;
            column = column < 0 ? 0 : column >= width ? width - 1 : column;
            
            std::uint8_t* pixel = input + row + column * channels;

            for (int k = 0; k < channels; k++)
            {
                accum[k] += static_cast<float>(pixel[k]);
            }
        }
    }

    std::uint8_t* px_o = output + (width * my + mx) * channels;

    for (int k = 0; k < channels; k++)
    {
        px_o[k] += static_cast<std::uint8_t>(accum[k] / samples);
    }
}
