#include "Config.hpp"

#include "ImageFilter\Parallel\ParallelBlurFastFilter.hpp"

#include "ImageProcess\Parallel\ParallelBlurSharpProcess.hpp"

ParallelBlurFastFilter::ParallelBlurFastFilter()
{
}

ParallelBlurFastFilter::~ParallelBlurFastFilter()
{
}

bool ParallelBlurFastFilter::onLoad(std::shared_ptr<ImageProcess> proc)
{
    std::shared_ptr<ParallelBlurSharpProcess> cst_proc = std::dynamic_pointer_cast<ParallelBlurSharpProcess>(proc);

    if (!cst_proc)
    {
        return false;
    }

    cl::Program prog = cst_proc->getProgram();

    kernel = cl::Kernel(prog, "blur_filter_fast");

    queue = cst_proc->getQueue();

    context = queue.getInfo<CL_QUEUE_CONTEXT>();

    first_run = true;

    return true;
}

void ParallelBlurFastFilter::onBefore(std::uint32_t glTexId)
{
    output = cl::BufferGL(context, CL_MEM_READ_WRITE, glTexId);

    std::vector<cl::Memory> objects;
    objects.push_back(output);

    queue.enqueueAcquireGLObjects(&objects);
}

std::uint64_t ParallelBlurFastFilter::onApply(const PPM & image)
{
    cl::Event event;

    cl::Buffer input = cl::Buffer(context, CL_MEM_READ_WRITE, image.getSize());

    if (first_run)
    {
        queue.enqueueWriteBuffer(input, CL_BLOCKING, 0, image.getSize(), image.getData());
    }
    else
    {
        queue.enqueueCopyBuffer(output, input, 0, 0, image.getSize());
    }

    kernel.setArg(0, input);
    kernel.setArg(1, output);
    kernel.setArg(2, image.getWidth());
    kernel.setArg(3, image.getHeight());

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

void ParallelBlurFastFilter::onAfter()
{
    std::vector<cl::Memory> objects;
    objects.push_back(output);

    queue.enqueueReleaseGLObjects(&objects);

    queue.finish();

    first_run = false;
}
