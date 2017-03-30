#include "Config.hpp"

#include "ImageFilter\Parallel\ParallelBlurFilter.hpp"

#include "ImageProcess\Parallel\ParallelBlurSharpProcess.hpp"

ParallelBlurFilter::ParallelBlurFilter()
{
}

ParallelBlurFilter::~ParallelBlurFilter()
{
}

bool ParallelBlurFilter::onLoad(std::shared_ptr<ImageProcess> proc)
{
    if (kernel())
    {
        return true;
    }

    std::shared_ptr<ParallelBlurSharpProcess> cst_proc = std::dynamic_pointer_cast<ParallelBlurSharpProcess>(proc);

    if (!cst_proc)
    {
        return false;
    }

    cl::Program prog = cst_proc->getProgram();

    kernel = cl::Kernel(prog, "blur_filter");

    queue = cst_proc->getQueue();

    context = queue.getInfo<CL_QUEUE_CONTEXT>();

    first_run = true;

    return true;
}

void ParallelBlurFilter::onBefore(std::uint32_t glTexId)
{
    output = cl::ImageGL(context, CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0, glTexId);

    std::vector<cl::Memory> objects;
    objects.push_back(output);

    glFinish();

    queue.enqueueAcquireGLObjects(&objects);
}

std::uint64_t ParallelBlurFilter::onApply(const PPM & image)
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

    cl::Image2D input = cl::Image2D(context,
        CL_MEM_READ_WRITE,
        cl::ImageFormat(CL_RGBA, CL_UNORM_INT8),
        image.getWidth(), image.getHeight());

    if (first_run)
    {
        queue.enqueueWriteImage(input, CL_BLOCKING, origin, region, 0, 0, image.getData());
    }
    else
    {
        queue.enqueueCopyImage(output, input, origin, origin, region);
    }

    kernel.setArg(0, input);
    kernel.setArg(1, output);

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

void ParallelBlurFilter::onAfter()
{
    std::vector<cl::Memory> objects;
    objects.push_back(output);

    queue.finish();

    queue.enqueueReleaseGLObjects(&objects);

    first_run = false;
}
