#include "Config.hpp"

#include "ImageProcess\Parallel\ParallelBlurSharpProcess.hpp"

#include "ImageFilter\Parallel\ParallelBlurFilter.hpp"
#include "ImageFilter\Parallel\ParallelSharpFilter.hpp"

#include "ImageFilter\Parallel\ParallelBlurFastFilter.hpp"
#include "ImageFilter\Parallel\ParallelSharpFastFilter.hpp"

#include "ImageFilter\Parallel\ParallelGaussianSharpFilter.hpp"

#include "ProgramEntry.hpp"

ParallelBlurSharpProcess::ParallelBlurSharpProcess(
    cl::Context context, std::uint32_t radius, 
    float alpha, float beta, float gamma,
    bool bandwith_optimisation)
    : 
    bandwith_optimisation(bandwith_optimisation),
    ParallelSharpProcess(context, radius, alpha, beta, gamma)
{
}

ParallelBlurSharpProcess::~ParallelBlurSharpProcess()
{
}

bool ParallelBlurSharpProcess::create()
{
    if (isReady())
    {
        return false;
    }

    std::shared_ptr<ImageFilter> blur_filter;

    if (bandwith_optimisation)
    {
        blur_filter = std::make_shared<ParallelBlurFastFilter>();
    }
    else
    {
        blur_filter = std::make_shared<ParallelBlurFilter>();
    }

    attachFilter(blur_filter);
    attachFilter(blur_filter);

    std::shared_ptr<ImageFilter> sharp_filter;

    if (bandwith_optimisation)
    {
        sharp_filter = std::make_shared<ParallelSharpFastFilter>();
    }
    else
    {
        sharp_filter = std::make_shared<ParallelSharpFilter>();
    }

    attachFilter(sharp_filter);

    return ParallelSharpProcess::create();
}

ProgramEntry ParallelBlurSharpProcess::getProgramSource()
{
    if (bandwith_optimisation)
    {
        return BlurSharpFast;
    }

    return BlurSharp;
}

void ParallelBlurSharpProcess::onBeforeExecute(PPM & image)
{
    if (!bandwith_optimisation)
    {
        image.convert(PPM::Format::RGBA);

        ParallelSharpProcess::onBeforeExecute(image);

        return;
    }

    GLuint pbo = getObjectGL();

    if (pbo)
    {
        glDeleteBuffers(1, &pbo);

        pbo = 0;
    }

    glGenBuffers(1, &pbo);

    glBindBuffer(GL_ARRAY_BUFFER, pbo);

    glBufferData(GL_ARRAY_BUFFER, image.getSize(), NULL, GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

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

    glTexImage2D(GL_TEXTURE_2D, 0, image.getGLInternalFormat(), 
        image.getWidth(), image.getHeight(), 0, 
        image.getGLFormat(), GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);

    setObjectGL(pbo);
}

void ParallelBlurSharpProcess::onAfterExecute()
{
    if (!bandwith_optimisation)
    {
        return;
    }

    GLuint pbo = getObjectGL();

    glBindTexture(GL_TEXTURE_2D, texId);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);

    int width, height, internalFormat;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

    GLenum format;

    switch (internalFormat)
    {
        case GL_R8: format = GL_R; break;
        case GL_RG8: format = GL_RG; break;
        case GL_RGB8: format = GL_RGB; break;
        case GL_RGBA8: format = GL_RGBA; break;
    }

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    if (pbo)
    {
        glDeleteBuffers(1, &pbo);

        pbo = 0;
    }

    setObjectGL(texId);
}
