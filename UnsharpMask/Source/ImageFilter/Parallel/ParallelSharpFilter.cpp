#include "Config.hpp"

#include "ImageFilter\Parallel\ParallelSharpFilter.hpp"

ParallelSharpFilter::ParallelSharpFilter()
{
}

ParallelSharpFilter::~ParallelSharpFilter()
{
}

bool ParallelSharpFilter::onLoad(std::shared_ptr<ImageProcess> proc)
{
    return false;
}

void ParallelSharpFilter::onBefore(std::uint32_t glTexId)
{
}

std::uint64_t ParallelSharpFilter::onApply(const PPM & image)
{
    return std::uint64_t();
}

void ParallelSharpFilter::onAfter()
{
}
