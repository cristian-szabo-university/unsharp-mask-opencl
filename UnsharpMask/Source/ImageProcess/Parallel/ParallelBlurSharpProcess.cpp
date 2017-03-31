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
    float alpha, float beta, float gamma)
    :
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

    attachFilter(std::make_shared<ParallelBlurFilter>(true));
    attachFilter(std::make_shared<ParallelBlurFilter>());
    attachFilter(std::make_shared<ParallelSharpFilter>());

    return ParallelSharpProcess::create();
}

ProgramEntry ParallelBlurSharpProcess::getProgramSource()
{
    return BlurSharp;
}

void ParallelBlurSharpProcess::onBeforeExecute(PPM & image)
{
    if (!hasImageFormat({ image.getCLFormat(), CL_UNSIGNED_INT8 }))
    {
        image.convert(PPM::Format::RGBA);
    }

    ParallelSharpProcess::onBeforeExecute(image);
}
