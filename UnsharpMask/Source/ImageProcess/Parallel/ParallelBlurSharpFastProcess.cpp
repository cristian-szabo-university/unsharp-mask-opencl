#include "Config.hpp"

#include "ImageProcess\Parallel\ParallelBlurSharpFastProcess.hpp"

#include "ImageFilter\Parallel\ParallelBlurFilter.hpp"
#include "ImageFilter\Parallel\ParallelSharpFilter.hpp"

#include "ImageFilter\Parallel\ParallelBlurFastFilter.hpp"
#include "ImageFilter\Parallel\ParallelSharpFastFilter.hpp"

#include "ImageFilter\Parallel\ParallelGaussianSharpFilter.hpp"

#include "ProgramEntry.hpp"

ParallelBlurSharpFastProcess::ParallelBlurSharpFastProcess(
    cl::Context context, std::uint32_t radius, 
    float alpha, float beta, float gamma)
    :
    ParallelSharpProcess(context, radius, alpha, beta, gamma)
{
}

ParallelBlurSharpFastProcess::~ParallelBlurSharpFastProcess()
{
}

bool ParallelBlurSharpFastProcess::create()
{
    if (isReady())
    {
        return false;
    }

    attachFilter(std::make_shared<ParallelBlurFastFilter>(true));
    attachFilter(std::make_shared<ParallelBlurFastFilter>());
    attachFilter(std::make_shared<ParallelSharpFastFilter>());

    return ParallelSharpProcess::create();
}

ProgramEntry ParallelBlurSharpFastProcess::getProgramSource()
{
    return BlurSharpFast;
}

void ParallelBlurSharpFastProcess::onBeforeExecute(PPM & image)
{
    glTexId = getObjectGL();

    if (glTexId)
    {
        glDeleteTextures(1, &glTexId);

        glTexId = 0;
    }

    GLuint pbo;

    glGenBuffers(1, &pbo);

    glBindBuffer(GL_ARRAY_BUFFER, pbo);

    glBufferData(GL_ARRAY_BUFFER, image.getSize(), NULL, GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    setObjectGL(pbo);

    glGenTextures(1, &glTexId);

    glBindTexture(GL_TEXTURE_2D, glTexId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, image.getGLInternalFormat(), 
        image.getWidth(), image.getHeight(), 0, 
        image.getGLFormat(), GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void ParallelBlurSharpFastProcess::onAfterExecute()
{
    GLuint pbo = getObjectGL();

    glBindTexture(GL_TEXTURE_2D, glTexId);

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

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    if (pbo)
    {
        glDeleteBuffers(1, &pbo);

        pbo = 0;
    }

    setObjectGL(glTexId);

    glBindTexture(GL_TEXTURE_2D, 0);

    glTexId = 0;
}
