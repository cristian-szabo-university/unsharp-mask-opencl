#include "Config.hpp"

#include "ImageProcess\ImageProcess.hpp"

#include "ImageFilter\ImageFilter.hpp"

ImageProcess::ImageProcess() : ready(false), objectId(0)
{
}

ImageProcess::~ImageProcess()
{
}

void ImageProcess::attachFilter(std::shared_ptr<ImageFilter> filter)
{
    filters.push_back(filter);
}

std::uint32_t ImageProcess::getObjectGL()
{
    return objectId;
}

bool ImageProcess::create()
{
    if (ready)
    {
        return false;
    }

    for (auto& filter : filters)
    {
        if (!filter->onLoad(shared_from_this()))
        {
            return false;
        }
    }

    ready = !ready;

    return true;
}

bool ImageProcess::isReady()
{
    return ready;
}

std::uint64_t ImageProcess::execute(const PPM & image)
{
    std::uint64_t exec_time = 0;

    if (!ready || !image.isReady())
    {
        return exec_time;
    }

    PPM copy = image;

    onBeforeExecute(copy);

    for (auto& filter : filters)
    {
        onBeforeFilter(filter, objectId);

        exec_time += onApplyFilter(filter, copy);

        onAfterFilter(filter);
    }

    onAfterExecute();

    return exec_time;
}

bool ImageProcess::destroy()
{
    if (!ready)
    {
        return false;
    }
    /*
    if (texId)
    {
        glDeleteTextures(1, &texId);

        texId = 0;
    }
    */
    ready = !ready;

    return true;
}

std::uint64_t ImageProcess::onApplyFilter(std::shared_ptr<ImageFilter> filter, const PPM& image)
{
    return filter->onApply(image);
}

void ImageProcess::setObjectGL(std::uint32_t objectId)
{
    this->objectId = objectId;
}

void ImageProcess::onBeforeExecute(PPM & image)
{
}

void ImageProcess::onAfterExecute()
{
}

void ImageProcess::onBeforeFilter(std::shared_ptr<ImageFilter> filter, std::uint32_t texId)
{
    filter->onBefore(texId);
}

void ImageProcess::onAfterFilter(std::shared_ptr<ImageFilter> filter)
{
    filter->onAfter();
}

/*
GLint internalFormat;
GLenum format;


if (texId)
{
glDeleteTextures(1, &texId);

texId = 0;
}

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

glGenTextures(1, &texId);

glBindTexture(GL_TEXTURE_2D, texId);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.getWidth(), image.getHeight(), 0, format, GL_UNSIGNED_BYTE, image.getData());

glBindTexture(GL_TEXTURE_2D, 0);
*/