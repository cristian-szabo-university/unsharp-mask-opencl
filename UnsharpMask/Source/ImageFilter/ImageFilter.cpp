#include "Config.hpp"

#include "ImageFilter\ImageFilter.hpp"

ImageFilter::~ImageFilter()
{
}

bool ImageFilter::onLoad(std::shared_ptr<ImageProcess> proc)
{
    return true;
}

void ImageFilter::onBefore(std::uint32_t glTexId)
{
}

void ImageFilter::onAfter()
{
}
