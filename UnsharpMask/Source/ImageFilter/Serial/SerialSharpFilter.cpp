#include "Config.hpp"

#include "ImageFilter\Serial\SerialSharpFilter.hpp"

#include "ImageProcess\Serial\SerialBlurSharpProcess.hpp"

#include "GL\glew.h"

#include "Utility.hpp"

SerialSharpFilter::SerialSharpFilter()
{
}

SerialSharpFilter::~SerialSharpFilter()
{
}

bool SerialSharpFilter::onLoad(std::shared_ptr<ImageProcess> proc)
{
    std::shared_ptr<SerialBlurSharpProcess> cast_proc = std::dynamic_pointer_cast<SerialBlurSharpProcess>(proc);

    if (!cast_proc)
    {
        return false;
    }

    alpha = cast_proc->getAlpha();

    beta = cast_proc->getBeta();

    gamma = cast_proc->getGamma();

    return true;
}

void SerialSharpFilter::onBefore(std::uint32_t glTexId)
{
    glBindTexture(GL_TEXTURE_2D, glTexId);

    GLint width, height, internalFormat;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

    if (previous.isReady())
    {
        previous.destroy();
    }

    PPM::Format format;

    switch (internalFormat)
    {
    case GL_RGBA8: format = PPM::Format::RGBA; break;
    case GL_RGB8: format = PPM::Format::RGB; break;
    case GL_R8: format = PPM::Format::INTENSITY; break;
    default: throw std::runtime_error("Invalid image format!");
    }

    previous.create(width, height, format);

    GLenum gl_format;

    switch (internalFormat)
    {
        case GL_RGBA8: gl_format = GL_RGBA; break;
        case GL_RGB8: gl_format = GL_RGB; break;
        case GL_R8: gl_format = GL_R; break;
        default: throw std::runtime_error("Invalid image format!");
    }

    glGetTexImage(GL_TEXTURE_2D, 0, gl_format, GL_UNSIGNED_BYTE, previous.getData());
}

std::uint64_t SerialSharpFilter::onApply(const PPM & image)
{
    PPM result;

    result.create(image.getWidth(), image.getHeight(), image.getFormat());

    std::uint8_t max = std::numeric_limits<std::uint8_t>::max();
    std::int32_t channels = image.getChannels();
    std::int32_t width = image.getWidth(), height = image.getHeight();
    std::uint8_t* in_data = static_cast<std::uint8_t*>(image.getData());
    std::uint8_t* blur_data = static_cast<std::uint8_t*>(previous.getData());
    std::uint8_t* out_data = static_cast<std::uint8_t*>(result.getData());

    auto start_time = std::chrono::high_resolution_clock::now();

    for (std::int32_t y = 0; y < height; ++y)
    {
        for (std::int32_t x = 0; x < width; ++x)
        {
            in_data += channels;
            blur_data += channels;
            out_data += channels;

            for (int k = 0; k < channels; k++)
            {
                float tmp = in_data[k] * alpha + blur_data[k] * beta + gamma;
                out_data[k] = tmp < 0 ? 0 : tmp > max ? max : (std::uint8_t)tmp;
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    GLint internalFormat;
    GLenum format;

    switch (image.getFormat())
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

    result.destroy();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
}

void SerialSharpFilter::onAfter()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
