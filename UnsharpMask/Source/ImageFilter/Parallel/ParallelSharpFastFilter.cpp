#include "Config.hpp"

#include "ImageFilter\Parallel\ParallelSharpFastFilter.hpp"

#include "ImageProcess\Parallel\ParallelBlurSharpFastProcess.hpp"

ParallelSharpFastFilter::ParallelSharpFastFilter()
{
}

ParallelSharpFastFilter::~ParallelSharpFastFilter()
{
}

bool ParallelSharpFastFilter::onLoad(std::shared_ptr<ImageProcess> proc)
{
    std::shared_ptr<ParallelBlurSharpFastProcess> cst_proc = std::dynamic_pointer_cast<ParallelBlurSharpFastProcess>(proc);

    if (!cst_proc)
    {
        return false;
    }

    cl::Program prog = cst_proc->getProgram();

    kernel = cl::Kernel(prog, "sharp_filter_fast");

    queue = cst_proc->getQueue();

    context = queue.getInfo<CL_QUEUE_CONTEXT>();

    return true;
}

void ParallelSharpFastFilter::onBefore(std::uint32_t glTexId)
{
    output = cl::BufferGL(context, CL_MEM_READ_WRITE, glTexId);

    std::vector<cl::Memory> objects;
    objects.push_back(output);

    queue.enqueueAcquireGLObjects(&objects);
}

std::uint64_t ParallelSharpFastFilter::onApply(const PPM & image)
{
    cl::Event event;

    cl::Buffer input1(context, CL_MEM_READ_WRITE, image.getSize());

    queue.enqueueWriteBuffer(input1, CL_BLOCKING, 0, image.getSize(), image.getData());

    cl::Buffer input2(context, CL_MEM_READ_WRITE, image.getSize());

    queue.enqueueCopyBuffer(output, input2, 0, 0, image.getSize());

    kernel.setArg(0, input1);
    kernel.setArg(1, input2);
    kernel.setArg(2, output);
    kernel.setArg(3, image.getWidth());
    kernel.setArg(4, image.getHeight());

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

void ParallelSharpFastFilter::onAfter()
{
    std::vector<cl::Memory> objects;
    objects.push_back(output);

    queue.enqueueReleaseGLObjects(&objects);

    queue.finish();
}
