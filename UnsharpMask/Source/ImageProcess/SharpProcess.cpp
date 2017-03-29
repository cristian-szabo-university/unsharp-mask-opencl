#include "Config.hpp"

#include "ImageProcess\SharpProcess.hpp"

#include "GL\glew.h"

SharpProcess::SharpProcess(std::uint32_t radius, float alpha, float beta, float gamma)
    : radius(radius), alpha(alpha), beta(beta), gamma(gamma)
{
}

SharpProcess::~SharpProcess()
{
}

std::uint32_t SharpProcess::getRadius()
{
    return radius;
}

float SharpProcess::getAlpha()
{
    return alpha;
}

float SharpProcess::getBeta()
{
    return beta;
}

float SharpProcess::getGamma()
{
    return gamma;
}

bool SharpProcess::destroy()
{
    if (!isReady())
    {
        return false;
    }

    std::uint32_t texId = getObjectGL();

    if (texId)
    {
        glDeleteTextures(1, &texId);

        texId = 0;
    }

    return ImageProcess::destroy();
}

void SharpProcess::onBeforeExecute(PPM & image)
{
    GLint internalFormat;
    GLenum format;

    std::uint32_t texId = getObjectGL();

    if (texId)
    {
        glDeleteTextures(1, &texId);

        texId = 0;
    }
    
    glGenTextures(1, &texId);

    setObjectGL(texId);

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
  
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.getWidth(), image.getHeight(), 0, format, GL_UNSIGNED_BYTE, image.getData());

    glBindTexture(GL_TEXTURE_2D, 0);
}
