#include "Config.hpp"

#include "ImageFilter\Parallel\ParallelBlurFilter.hpp"

ParallelBlurFilter::ParallelBlurFilter()
{
}

ParallelBlurFilter::~ParallelBlurFilter()
{
}

bool ParallelBlurFilter::onLoad(std::shared_ptr<ImageProcess> proc)
{
    return true;
}

void ParallelBlurFilter::onBefore(std::uint32_t glTexId)
{
}

std::uint64_t ParallelBlurFilter::onApply(const PPM & image)
{
    return 0;
}

void ParallelBlurFilter::onAfter()
{
}
