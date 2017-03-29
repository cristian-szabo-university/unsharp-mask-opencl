#include "Config.hpp"

#include "ImageProcess\Parallel\ParallelGaussianSharpProcess.hpp"

#include "ImageFilter\Parallel\ParallelGaussianSharpFilter.hpp"

#include "ProgramEntry.hpp"

ParallelGaussianSharpProcess::ParallelGaussianSharpProcess(cl::Context context, std::uint32_t radius, float alpha, float beta, float gamma)
    : ParallelSharpProcess(context, radius, alpha, beta, gamma)
{
}

ParallelGaussianSharpProcess::~ParallelGaussianSharpProcess()
{
}

bool ParallelGaussianSharpProcess::create()
{
    if (isReady())
    {
        return false;
    }

    attachFilter(std::make_shared<ParallelGaussianSharpFilter>());

    return ParallelSharpProcess::create();
}

ProgramEntry ParallelGaussianSharpProcess::getProgramSource()
{
    return GaussianSharp;
}

void ParallelGaussianSharpProcess::onBeforeExecute(PPM & image)
{
    image.convert(PPM::Format::RGBA);

    ParallelSharpProcess::onBeforeExecute(image);
}
