#include "Config.hpp"

#include "ImageProcess\Parallel\ParallelBlurSharpProcess.hpp"

#include "ImageFilter\Serial\SerialBlurFilter.hpp"
#include "ImageFilter\Serial\SerialSharpFilter.hpp"

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

    for (std::size_t i = 1; i < 3; ++i)
    {
        std::shared_ptr<ImageFilter> filter;

        if (bandwith_optimisation)
        {
            filter = std::make_shared<ParallelBlurFastFilter>();
        }
        else
        {
            filter = std::make_shared<ParallelBlurFilter>();
        }

        attachFilter(filter);
    }

    attachFilter(std::make_shared<ParallelSharpFastFilter>());

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

std::uint64_t ParallelBlurSharpProcess::onApplyFilter(std::shared_ptr<ImageFilter> filter, const PPM & image)
{
    PPM buffer = image;

    if (!bandwith_optimisation)
    {
        buffer.convert(PPM::Format::RGBA);
    }

    return ImageProcess::onApplyFilter(filter, buffer);
}
