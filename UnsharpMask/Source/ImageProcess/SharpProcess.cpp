#include "Config.hpp"

#include "ImageProcess\SharpProcess.hpp"

SharpProcess::SharpProcess(std::uint32_t radius, float alpha, float beta, float gamma)
    : radius(radius), alpha(alpha), beta(beta), gamma(gamma)
{
}

SharpProcess::~SharpProcess()
{
}

void SharpProcess::setRadius(std::uint32_t radius)
{
    this->radius = radius;

    onRadiusChange();
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
    std::uint32_t texId = getObjectGL();

    if (texId)
    {
        glDeleteTextures(1, &texId);

        texId = 0;
    }
    
    glGenTextures(1, &texId);

    glBindTexture(GL_TEXTURE_2D, texId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, image.getGLInternalFormat(), image.getWidth(), image.getHeight(), 0, image.getGLFormat(), GL_UNSIGNED_BYTE, image.getData());

    glBindTexture(GL_TEXTURE_2D, 0);

    setObjectGL(texId);
}

void SharpProcess::onRadiusChange()
{
    reloadFilters();
}
