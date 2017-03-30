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
        filter->onBefore(objectId);

        exec_time += filter->onApply(copy);

        filter->onAfter();
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

    ready = !ready;

    return true;
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
