#include "Config.hpp"

#include "ImageFilter\Parallel\ParallelGaussianSharpFilter.hpp"

#include "ImageProcess\Parallel\ParallelGaussianSharpProcess.hpp"

ParallelGaussianSharpFilter::ParallelGaussianSharpFilter()
{
}

ParallelGaussianSharpFilter::~ParallelGaussianSharpFilter()
{
}

bool ParallelGaussianSharpFilter::onLoad(std::shared_ptr<ImageProcess> proc)
{
    if (kernel())
    {
        return true;
    }

    std::shared_ptr<ParallelGaussianSharpProcess> cst_proc = std::dynamic_pointer_cast<ParallelGaussianSharpProcess>(proc);

    if (!cst_proc)
    {
        return false;
    }

    cl::Program prog = cst_proc->getProgram();

    kernel = cl::Kernel(prog, "gaussian_sharp_filter");

    queue = cst_proc->getQueue();

    context = queue.getInfo<CL_QUEUE_CONTEXT>();

    std::int32_t radius = cst_proc->getRadius();

    std::vector<float> gaussian_weights = createFilter(radius, radius / 2.0f);
    std::size_t size = sizeof(float) * gaussian_weights.size();

    filter = cl::Buffer(context, CL_MEM_READ_ONLY, size);

    queue.enqueueWriteBuffer(filter, CL_BLOCKING, 0, size, gaussian_weights.data());

    return true;
}

void ParallelGaussianSharpFilter::onBefore(std::uint32_t glTexId)
{
    output = cl::ImageGL(context, CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0, glTexId);

    std::vector<cl::Memory> objects;
    objects.push_back(output);

    glFinish();

    queue.enqueueAcquireGLObjects(&objects);
}

std::uint64_t ParallelGaussianSharpFilter::onApply(const PPM & image)
{
    cl::Event event;

    cl::size_t<3> origin;
    origin[0] = 0;
    origin[1] = 0;
    origin[2] = 0;

    cl::size_t<3> region;
    region[0] = image.getWidth();
    region[1] = image.getHeight();
    region[2] = 1;

    cl::Image2D input1(context,
        CL_MEM_READ_WRITE,
        cl::ImageFormat(CL_RGBA, CL_UNORM_INT8),
        image.getWidth(), image.getHeight());

    queue.enqueueWriteImage(input1, CL_BLOCKING, origin, region, 0, 0, image.getData());

    kernel.setArg(0, input1);
    kernel.setArg(1, filter);
    kernel.setArg(2, output);

    queue.enqueueNDRangeKernel(
        kernel,
        cl::NullRange,
        cl::NDRange(image.getWidth(), image.getHeight()),
        cl::NullRange,
        NULL, &event);

    event.wait();

    std::int64_t start_time = event.getProfilingInfo<CL_PROFILING_COMMAND_START>();

    std::int64_t end_time = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();

    return (end_time - start_time);
}

void ParallelGaussianSharpFilter::onAfter()
{
    std::vector<cl::Memory> objects;
    objects.push_back(output);

    queue.finish();

    queue.enqueueReleaseGLObjects(&objects);
}

std::vector<float> ParallelGaussianSharpFilter::createFilter(std::int32_t radius, float sigma)
{
    std::vector<float> result;

    std::size_t filter_size = radius * 2 + 1;
    result.resize(filter_size * filter_size);

    std::size_t index = 0;
    float sum = 0.0f;

    for (std::int32_t y = -radius; y <= radius; y++)
    {
        for (std::int32_t x = -radius; x <= radius; x++, index++)
        {
            float temp = std::exp(-(std::powf((float)x, 2.0f) + std::powf((float)y, 2.0f) / (2.0f * std::powf(sigma, 2.0f))));

            result[index] = temp;

            sum += temp;
        }
    }

    for (auto& mask : result)
    {
        mask /= sum;
    }

    return result;
}